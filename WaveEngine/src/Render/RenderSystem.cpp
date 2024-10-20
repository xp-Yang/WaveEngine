#include "RenderSystem.hpp"

#include "Path/ForwardRenderPath.hpp"
#include "Path/DeferredRenderPath.hpp"
#include "Path/RayTracingRenderPath.hpp"

#include <Logical/Framework/Scene.hpp>

RenderSystem::RenderSystem()
{
    RenderSourceData::initRHI();
}

void RenderSystem::init(std::shared_ptr<Scene> scene)
{
    m_scene = scene;

    m_render_source_data = std::make_shared<RenderSourceData>();

    m_forward_path = std::make_shared<ForwardRenderPath>(this);
    m_forward_path->prepareRenderSourceData(m_render_source_data);
    m_forward_path->init();

    m_deferred_path = std::make_shared<DeferredRenderPath>(this);
    m_deferred_path->prepareRenderSourceData(m_render_source_data);
    m_deferred_path->init();

    m_ray_tracing_path = std::make_shared<RayTracingRenderPath>();
    m_ray_tracing_path->prepareRenderSourceData(m_render_source_data);
    m_ray_tracing_path->init();

    setRenderPathType(RenderPathType::Deferred);

    m_gcode_viewer = std::make_shared<GcodeViewer>();
}

RenderPathType RenderSystem::getRenderPathType()
{
    return m_path_type;
}

void RenderSystem::setRenderPathType(RenderPathType type)
{
    m_path_type = type;

    switch (m_path_type)
    {
    case RenderPathType::Forward:
        m_curr_path = m_forward_path;
        break;
    case RenderPathType::Deferred:
        m_curr_path = m_deferred_path;
        break;
    case RenderPathType::RayTracing:
        m_curr_path = m_ray_tracing_path;
        break;
    default:
        break;
    }
}

RenderParams& RenderSystem::renderParams()
{
    return m_render_params;
}

void RenderSystem::onUpdate()
{
    updateRenderSourceData();

    m_curr_path->render();
}

unsigned int RenderSystem::getPickingFBO()
{
    return m_curr_path->getPickingFBO();
}

unsigned int RenderSystem::renderPassTexture(RenderPass::Type render_pass_type)
{
    RhiTexture* texture = m_curr_path->renderPassTexture(render_pass_type);
    return texture ? texture->id() : 0;
}

void RenderSystem::updateRenderSourceData()
{
    if (!m_initialized) {
        std::shared_ptr<Mesh> screen_quad_sub_mesh;
        screen_quad_sub_mesh = Mesh::create_screen_mesh();
        m_render_source_data->screen_quad = std::make_shared<RenderMeshData>(screen_quad_sub_mesh);
    }

    if (!m_initialized) {
        const auto& dir_light = m_scene->getLightManager()->mainDirectionalLight();
        m_render_source_data->render_directional_light_data_list.emplace_back(
            RenderDirectionalLightData{ dir_light->luminousColor, dir_light->direction,
            dir_light->lightViewMatrix(), dir_light->lightProjMatrix() });
    }

    const auto& point_lights = m_scene->getLightManager()->pointLights();
    struct inst_data {
        Mat4 inst_matrix;
        Color4 inst_color;
    };
    static inst_data* point_light_inst_data = new inst_data[point_lights.size()]{};
    if (!m_initialized) {
        for (int i = 0; i < point_lights.size(); ++i) {
            point_light_inst_data[i].inst_matrix = Math::Translate(point_lights[i]->position);
            point_light_inst_data[i].inst_color = point_lights[i]->luminousColor;
        }

        //std::shared_ptr<Mesh> point_light_mesh = Mesh::create_icosphere_mesh(0.05f, 4);
        //m_render_source_data->render_point_light_inst_mesh = std::make_shared<RenderMeshNode>(RenderMeshDataID(-99999, 0), point_light_mesh, Mat4(1.0));
        //m_render_source_data->render_point_light_inst_mesh->create_instancing(point_light_inst_data, point_lights.size() * sizeof(inst_data));
        m_render_source_data->point_light_inst_amount = point_lights.size();
    }
    for (const auto& point_light : point_lights) {
        Mat4 point_light_matrix = Math::Translate(point_light->position);
        int point_light_id = point_light->ID().id;
        auto it = std::find_if(m_render_source_data->render_point_light_data_list.begin(), m_render_source_data->render_point_light_data_list.end(),
            [point_light_id](const RenderPointLightData& point_light_data) {
                return point_light_data.id == point_light_id;
            }
        );
        if (it != m_render_source_data->render_point_light_data_list.end())
        {
            it->position = point_light->position;
        }
        else {
            m_render_source_data->render_point_light_data_list.emplace_back(
                RenderPointLightData{ point_light_id, point_light->luminousColor, point_light->position, point_light->radius,
                point_light->lightViewMatrix(), point_light->lightProjMatrix() });
        }
    }

    if (!m_initialized) {
        std::shared_ptr<Mesh> skybox_mesh = Mesh::create_cube_mesh();
        const std::string resource_dir = RESOURCE_DIR;
        CubeTexture skybox_cube_texture = CubeTexture(
            resource_dir + "/images/skybox/right.jpg",
            resource_dir + "/images/skybox/left.jpg",
            resource_dir + "/images/skybox/top.jpg",
            resource_dir + "/images/skybox/bottom.jpg",
            resource_dir + "/images/skybox/front.jpg",
            resource_dir + "/images/skybox/back.jpg");
        m_render_source_data->render_skybox_node.skybox_cube_map = RenderTextureData(skybox_cube_texture).id;
        m_render_source_data->render_skybox_node.mesh = std::make_shared<RenderMeshData>(skybox_mesh);
    }

    const auto& objects = m_scene->getObjects();
    for (const auto& object : objects) {
        auto& sub_meshes = object->getComponent<MeshComponent>()->sub_meshes;
        auto& model_matrix = object->getComponent<TransformComponent>()->transform();
        bool visible = object->visible();
        for (const auto& sub_mesh : sub_meshes) {
            auto render_mesh_data_id = RenderMeshNodeID(object->ID(), sub_mesh->sub_mesh_idx);
            auto it = m_render_source_data->render_mesh_nodes.find(render_mesh_data_id);
            if (!visible) {
                if (it != m_render_source_data->render_mesh_nodes.end())
                    m_render_source_data->render_mesh_nodes.erase(render_mesh_data_id);
            }
            else {
                if (it != m_render_source_data->render_mesh_nodes.end()) {
                    m_render_source_data->render_mesh_nodes[render_mesh_data_id]->model_matrix = (model_matrix * sub_mesh->local_transform);
                    m_render_source_data->render_mesh_nodes[render_mesh_data_id]->updateRenderMaterialData(sub_mesh->material);
                }
                else {
                    m_render_source_data->render_mesh_nodes.insert_or_assign(render_mesh_data_id,
                        std::make_shared<RenderMeshNode>(render_mesh_data_id, RenderMeshData(sub_mesh), RenderMaterialData(sub_mesh->material), model_matrix));
                }
            }
        }
    }

    // picked
    m_render_source_data->picked_ids.clear();
    for (const auto& object : m_scene->getPickedObjects()) {
        m_render_source_data->picked_ids.push_back(object->ID());
    }

    CameraComponent& camera = m_scene->getMainCamera();
    m_render_source_data->camera_position = camera.pos;
    m_render_source_data->view_matrix = camera.view;
    m_render_source_data->proj_matrix = camera.projection;

    if (!m_render_source_data->render_camera)
        m_render_source_data->render_camera = std::make_shared<RenderCameraData>();
    m_render_source_data->render_camera->fov = camera.fov;
    m_render_source_data->render_camera->pos = camera.pos;
    m_render_source_data->render_camera->direction = camera.direction;
    m_render_source_data->render_camera->upDirection = camera.upDirection;
    m_render_source_data->render_camera->rightDirection = camera.getRightDirection();

    m_initialized = true;

    if (m_render_source_data->gcode_mesh_data.empty() && !m_gcode_viewer->meshes().empty())
        m_render_source_data->gcode_mesh_data = m_gcode_viewer->meshes();
}
