#include "RenderSystem.hpp"
#include "Path/ForwardRenderPath.hpp"
#include "Path/DeferredRenderPath.hpp"
#include "Path/RayTracingRenderPath.hpp"

#include "Logical/Framework/ECS/Components.hpp"

RenderSystem::RenderSystem()
{
    m_screen_quad_sub_mesh.sub_mesh_data = Asset::MeshData::create_screen_mesh();
    m_screen_quad = std::make_shared<RenderSubMeshData>(m_screen_quad_sub_mesh);

    m_render_source_data = std::make_shared<RenderSourceData>();

    m_forward_path = std::make_shared<ForwardRenderPath>();
    m_forward_path->init();
    m_forward_path->prepareScreenQuadData(m_screen_quad);
    m_forward_path->prepareRenderSourceData(m_render_source_data);

    m_deferred_path = std::make_shared<DeferredRenderPath>();
    m_deferred_path->init();
    m_deferred_path->prepareScreenQuadData(m_screen_quad);
    m_deferred_path->prepareRenderSourceData(m_render_source_data);

    m_ray_tracing_path = std::make_shared<RayTracingRenderPath>();
    m_ray_tracing_path->init();
    m_ray_tracing_path->prepareScreenQuadData(m_screen_quad);
    m_ray_tracing_path->prepareRenderSourceData(m_render_source_data);

    setRenderPathType(RenderPath::Type::Deferred);
}

RenderPath::Type RenderSystem::getRenderPathType()
{
    return m_path_type;
}

void RenderSystem::setRenderPathType(RenderPath::Type type)
{
    m_path_type = type;

    switch (m_path_type)
    {
    case RenderPath::Type::Forward:
        m_curr_path = m_forward_path;
        break;
    case RenderPath::Type::Deferred:
        m_curr_path = m_deferred_path;
        break;
    case RenderPath::Type::RayTracing:
        m_curr_path = m_ray_tracing_path;
        break;
    default:
        break;
    }
}

void RenderSystem::setRenderParams(const RenderParams& params)
{
    m_curr_path->setRenderParams(params);
}

void RenderSystem::onUpdate()
{
    updateRenderSourceData();

    m_curr_path->render();
}

void RenderSystem::updateRenderSourceData()
{
    auto& world = ecs::World::get();

    // TODO need_process_source_data if GObject dynamically construct or destruct
    if (m_need_process_source_data) {
        // 相对静态的数据
        // 1. object mesh
        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
            if (world.hasComponent<ecs::PointLightComponent>(entity) ||
                world.hasComponent<ecs::SkyboxComponent>(entity))
                continue;

            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            for (const auto& sub_mesh : sub_meshes) {
                m_render_source_data->render_object_sub_mesh_data_list.emplace_back(std::make_shared<RenderSubMeshData>(sub_mesh, model_matrix));
            }
        }
        // 2. skybox
        m_render_source_data->render_skybox_data.skybox_cube_map = world.getSkyboxComponent()->texture;
        for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            for (const auto& sub_mesh : sub_meshes) {
                m_render_source_data->render_skybox_data.render_sub_mesh_data = std::make_shared<RenderSubMeshData>(sub_mesh, model_matrix);
            }
            break;
        }
        // 3. light mesh
        for (auto entity : world.entityView<ecs::PointLightComponent>()) {
            auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            m_render_source_data->render_point_light_data_list.emplace_back(
                RenderPointLightData{ point_light.luminousColor, point_light.position(), point_light.radius, point_light.lightReferenceMatrix(),
                std::make_shared<RenderSubMeshData>(sub_meshes.front(), model_matrix) });
        }
        auto& dir_light_component = *world.getMainDirectionalLightComponent();
        m_render_source_data->render_directional_light_data_list.emplace_back(
            RenderDirectionalLightData{ dir_light_component.luminousColor, dir_light_component.direction, dir_light_component.lightReferenceMatrix() });
    }

    // 动态的数据
    // 1. object model matrix
    int i = 0;
    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if (world.hasComponent<ecs::PointLightComponent>(entity) ||
            world.hasComponent<ecs::SkyboxComponent>(entity))
            continue;
        auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
        auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
        for (const auto& sub_mesh : sub_meshes) {
            m_render_source_data->render_object_sub_mesh_data_list[i]->updateTransform(model_matrix * sub_mesh.local_transform);
            i++; // TODO 之后使用GObjectID来索引
        }
        // TODO need updateRenderMaterialData if changed
        //m_render_source_data->render_object_sub_mesh_data_list[i].updateRenderMaterialData(sub_meshes[i].material);
    }
    // 2. light model matrix
    i = 0;
    for (auto entity : world.entityView<ecs::PointLightComponent>()) {
        auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
        auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
        auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
        m_render_source_data->render_point_light_data_list[i].color = point_light.luminousColor;
        m_render_source_data->render_point_light_data_list[i].position = point_light.position();
        m_render_source_data->render_point_light_data_list[i].radius = point_light.radius;
        m_render_source_data->render_point_light_data_list[i].lightReferenceMatrix = point_light.lightReferenceMatrix();
        m_render_source_data->render_point_light_data_list[i].render_sub_mesh_data->updateTransform(model_matrix * sub_meshes.front().local_transform);
        i++;
    }
    i = 0;
    for (auto entity : world.entityView<ecs::DirectionalLightComponent>()) {
        auto& dir_light_component = *world.getComponent<ecs::DirectionalLightComponent>(entity);
        m_render_source_data->render_directional_light_data_list[i].color = dir_light_component.luminousColor;
        m_render_source_data->render_directional_light_data_list[i].direction = dir_light_component.direction;
        m_render_source_data->render_directional_light_data_list[i].lightReferenceMatrix = dir_light_component.lightReferenceMatrix();
        i++;
    }
    // 3. camera
    auto& camera = *world.getMainCameraComponent();
    m_render_source_data->camera_position = camera.pos;
    m_render_source_data->view_matrix = camera.view;
    m_render_source_data->proj_matrix = camera.projection;

    m_need_process_source_data = false;
}
