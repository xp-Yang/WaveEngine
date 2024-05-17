#include "RenderSystem.hpp"

#include "Path/ForwardRenderPath.hpp"
#include "Path/DeferredRenderPath.hpp"
#include "Path/RayTracingRenderPath.hpp"

#if ENABLE_ECS
#include "Logical/Framework/ECS/Components.hpp"
#endif

#include <Logical/Framework/Scene.hpp>

RenderSystem::RenderSystem()
{
    m_screen_quad_sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomScreen, "" };
    m_screen_quad = std::make_shared<RenderSubMeshData>(-1, m_screen_quad_sub_mesh);

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

#if ENABLE_ECS
    connect(&(ecs::World::get()), &ecs::World::get().componentInserted, this, &RenderSystem::onComponentInserted);
    connect(&(ecs::World::get()), &ecs::World::get().componentRemoved, this, &RenderSystem::onComponentRemoved);
#endif
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

#if ENABLE_ECS
void RenderSystem::onComponentInserted(int entt_id, int pool_id)
{
    auto& world = ecs::World::get();
    if (!world.getComponent<ecs::RenderableComponent>(entt_id) && !world.hasComponent<ecs::PointLightComponent>(entt_id))
        return;
    if (world.getComponent<ecs::RenderableComponent>(entt_id)) {
        auto it = std::find_if(m_render_source_data->render_object_sub_mesh_data_list.begin(), m_render_source_data->render_object_sub_mesh_data_list.end(),
            [entt_id](auto& render_object_sub_mesh_data) {
                return render_object_sub_mesh_data->id() == entt_id;
            }
        );
        if (it == m_render_source_data->render_object_sub_mesh_data_list.end()) {
            m_need_insert_source_data = true;
            m_need_insert_id = entt_id;
        }
    }
    if (world.getComponent<ecs::PointLightComponent>(entt_id)) {
        auto it = std::find_if(m_render_source_data->render_point_light_data_list.begin(), m_render_source_data->render_point_light_data_list.end(),
            [entt_id](RenderPointLightData& render_point_light_data) {
                return render_point_light_data.render_sub_mesh_data->id() == entt_id;
            }
        );
        if (it == m_render_source_data->render_point_light_data_list.end()) {
            m_need_insert_source_data = true;
            m_need_insert_id = entt_id;
        }
    }
}

void RenderSystem::onComponentRemoved(int entt_id, int pool_id)
{
    auto& world = ecs::World::get();
    if (world.getComponent<ecs::RenderableComponent>(entt_id) || world.hasComponent<ecs::PointLightComponent>(entt_id))
        return;
    m_need_remove_source_data = true;
    m_need_remove_id = entt_id;
}
#else
void RenderSystem::onComponentInserted(int entt_id, int pool_id)
{

}

void RenderSystem::onComponentRemoved(int entt_id, int pool_id)
{

}
#endif // ENABLE_ECS

#if ENABLE_ECS
void RenderSystem::updateRenderSourceData()
{
    auto& world = ecs::World::get();

    // TODO need_process_source_data if GObject dynamically construct or destruct
    if (!m_initialized) {
        m_render_source_data->reset();
        // 相对静态的数据
        // 1. object mesh
        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(entity)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            for (const auto& sub_mesh : sub_meshes) {
                m_render_source_data->render_object_sub_mesh_data_list.emplace_back(std::make_shared<RenderSubMeshData>(entity.getId(), sub_mesh, model_matrix));
            }
        }
        // 2. skybox
        for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
            auto& skybox_mesh = world.getComponent<ecs::SkyboxComponent>(entity)->mesh;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            m_render_source_data->render_skybox_data.skybox_cube_map = RenderTextureData(world.getComponent<ecs::SkyboxComponent>(entity)->cube_texture).map;
            m_render_source_data->render_skybox_data.render_sub_mesh_data = std::make_shared<RenderSubMeshData>(entity.getId(), skybox_mesh, model_matrix);
            break;
        }
        // 3. light mesh
        for (auto entity : world.entityView<ecs::PointLightComponent>()) {
            auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
            m_render_source_data->render_point_light_data_list.emplace_back(
                RenderPointLightData{ point_light.luminousColor, point_light.position(), point_light.radius, point_light.lightReferenceMatrix(),
                std::make_shared<RenderSubMeshData>(entity.getId(), point_light.mesh, model_matrix) });
        }
        for (auto entity : world.entityView<ecs::DirectionalLightComponent>()) {
            auto& dir_light_component = *world.getComponent<ecs::DirectionalLightComponent>(entity);
            m_render_source_data->render_directional_light_data_list.emplace_back(
                RenderDirectionalLightData{ dir_light_component.luminousColor, dir_light_component.direction, dir_light_component.lightReferenceMatrix() });
        }
    }

    if (m_need_insert_source_data) {
        // 1. insert
        if (world.hasComponent<ecs::PointLightComponent>(m_need_insert_id))
        {
            auto& point_light = *world.getComponent<ecs::PointLightComponent>(m_need_insert_id);
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(m_need_insert_id)->transform();
            m_render_source_data->render_point_light_data_list.emplace_back(
                RenderPointLightData{ point_light.luminousColor, point_light.position(), point_light.radius, point_light.lightReferenceMatrix(),
                std::make_shared<RenderSubMeshData>(m_need_insert_id, point_light.mesh, model_matrix) });
        }
        if (world.hasComponent<ecs::RenderableComponent>(m_need_insert_id))
        {
            auto& sub_meshes = world.getComponent<ecs::RenderableComponent>(m_need_insert_id)->sub_meshes;
            auto& model_matrix = world.getComponent<ecs::TransformComponent>(m_need_insert_id)->transform();
            for (const auto& sub_mesh : sub_meshes) {
                m_render_source_data->render_object_sub_mesh_data_list.emplace_back(std::make_shared<RenderSubMeshData>(m_need_insert_id, sub_mesh, model_matrix));
            }
        }

        m_need_insert_source_data = false;
        m_need_insert_id = -1;
    }

    if (m_need_remove_source_data) {
        // 2. remove
        auto it = std::find_if(m_render_source_data->render_object_sub_mesh_data_list.begin(), m_render_source_data->render_object_sub_mesh_data_list.end(),
            [this](auto& render_object_sub_mesh_data) {
                return render_object_sub_mesh_data->id() == m_need_remove_id;
            }
        );
        if (it != m_render_source_data->render_object_sub_mesh_data_list.end()) {
            m_render_source_data->render_object_sub_mesh_data_list.erase(it);
        }

        auto it_point_light = std::find_if(m_render_source_data->render_point_light_data_list.begin(), m_render_source_data->render_point_light_data_list.end(),
            [this](auto& render_point_light_data) {
                return render_point_light_data.render_sub_mesh_data->id() == m_need_remove_id;
            }
        );
        if (it_point_light != m_render_source_data->render_point_light_data_list.end()) {
            m_render_source_data->render_point_light_data_list.erase(it_point_light);
        }

        m_need_remove_source_data = false;
        m_need_remove_id = -1;
    }

    // 动态的数据
    // 1. object model matrix
    int i = 0;
    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
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
        auto& model_matrix = world.getComponent<ecs::TransformComponent>(entity)->transform();
        m_render_source_data->render_point_light_data_list[i].color = point_light.luminousColor;
        m_render_source_data->render_point_light_data_list[i].position = point_light.position();
        m_render_source_data->render_point_light_data_list[i].radius = point_light.radius;
        m_render_source_data->render_point_light_data_list[i].lightReferenceMatrix = point_light.lightReferenceMatrix();
        m_render_source_data->render_point_light_data_list[i].render_sub_mesh_data->updateTransform(model_matrix * point_light.mesh.local_transform);
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
    // 3. picked
    m_render_source_data->picked_ids.clear();
    for (const auto& entity : world.getPickedEntities()) {
        m_render_source_data->picked_ids.push_back(entity.getId());
    }
    // 4. camera
    auto& camera = *world.getMainCameraComponent();
    m_render_source_data->camera_position = camera.pos;
    m_render_source_data->view_matrix = camera.view;
    m_render_source_data->proj_matrix = camera.projection;

    m_initialized = true;
}
#else
void RenderSystem::updateRenderSourceData()
{
    auto& scene = *Application::GetApp().getScene();
    if (!m_initialized) {
        const auto& objects = scene.getObjects();
        for (const auto& object : objects) {
            auto& sub_meshes = object->getComponent<MeshComponent>()->sub_meshes;
            auto& model_matrix = object->getComponent<TransformComponent>()->transform();
            for (const auto& sub_mesh : sub_meshes) {
                m_render_source_data->render_object_sub_mesh_data_list.emplace_back(std::make_shared<RenderSubMeshData>(object->ID(), sub_mesh, model_matrix));
            }
        }

        const auto& dir_light = scene.getLightManager()->mainDirectionalLight();
        m_render_source_data->render_directional_light_data_list.emplace_back(
            RenderDirectionalLightData{ dir_light->luminousColor, dir_light->direction, dir_light->lightReferenceMatrix() });

        const auto& point_lights = scene.getLightManager()->pointLights();
        for (const auto& point_light : point_lights) {
            Asset::SubMesh point_light_mesh;
            point_light_mesh.mesh_file_ref = { Asset::MeshFileType::CustomSphere, "" };
            Mat4 point_light_matrix = Math::Translate(point_light->position);
            m_render_source_data->render_point_light_data_list.emplace_back(
                RenderPointLightData{ point_light->luminousColor, point_light->position, point_light->radius, point_light->lightReferenceMatrix(),
                std::make_shared<RenderSubMeshData>(-2, point_light_mesh, point_light_matrix) });
        }

        m_render_source_data->render_camera = std::make_shared<RenderCameraData>();
    }

    auto& camera = scene.getMainCamera();
    m_render_source_data->camera_position = camera.pos;
    m_render_source_data->view_matrix = camera.view;
    m_render_source_data->proj_matrix = camera.projection;

    m_render_source_data->render_camera->fov = camera.fov;
    m_render_source_data->render_camera->pos = camera.pos;
    m_render_source_data->render_camera->direction = camera.direction;
    m_render_source_data->render_camera->upDirection = camera.upDirection;
    m_render_source_data->render_camera->rightDirection = camera.getRightDirection();

    m_initialized = true;
}
#endif // ENABLE_ECS
