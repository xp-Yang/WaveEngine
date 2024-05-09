#include "RenderSystem.hpp"
#include "Path/ForwardRenderPath.hpp"
#include "Path/DeferredRenderPath.hpp"
#include "Path/RayTracingRenderPath.hpp"

#include "Logical/Framework/ECS/Components.hpp"

RenderSystem::RenderSystem()
{
    Asset::SubMesh screen_quad_sub_mesh;
    screen_quad_sub_mesh.sub_mesh_data = Asset::MeshData::create_screen_mesh();
    m_screen_quad = std::make_shared<RenderSubMeshData>(screen_quad_sub_mesh);

    m_render_source_data = std::make_shared<RenderSourceData>();

    m_forward_path = std::make_shared<ForwardRenderPath>();
    m_forward_path->init();
    m_forward_path->prepareRenderSourceData(m_render_source_data);

    m_deferred_path = std::make_shared<DeferredRenderPath>();
    m_deferred_path->init();
    m_deferred_path->prepareRenderSourceData(m_render_source_data);

    m_ray_tracing_path = std::make_shared<RayTracingRenderPath>();
    m_ray_tracing_path->init();
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
    ecs::CameraComponent& camera = *world.getMainCameraComponent();

    m_render_source_data->view_matrix = camera.view;
    m_render_source_data->proj_matrix = camera.projection;

    if (m_need_process_source_data) {
        // 相对静态的数据
        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
            if (world.hasComponent<ecs::PointLightComponent>(entity) ||
                world.hasComponent<ecs::SkyboxComponent>(entity) /*||
                world.getComponent<ecs::NameComponent>(entity)->name == "Grid"*/)
                continue;

            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            m_render_source_data->render_mesh_data_list.emplace_back(sub_meshes, model_matrix);
        }
        m_render_source_data->render_skybox_data.skybox_cube_map = world.getSkyboxComponent()->texture;
    }

    int i = 0;
    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
        m_render_source_data->render_mesh_data_list[i].model_matrix = model_matrix;
        i++; // TODO 之后使用GObjectID来索引
    }
    for (auto entity : world.entityView<ecs::PointLightComponent>()) {
        auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
        auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
        auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
        m_render_source_data->render_point_light_data_list.emplace_back(
            point_light.luminousColor, point_light.position(), point_light.radius, point_light.lightReferenceMatrix(), RenderMeshData(sub_meshes, model_matrix));
    }
    auto& dir_light_component = *world.getMainDirectionalLightComponent();
    m_render_source_data->render_directional_light_data_list.emplace_back(
        dir_light_component.luminousColor, dir_light_component.direction, dir_light_component.lightReferenceMatrix());

    m_need_process_source_data = false;
}
