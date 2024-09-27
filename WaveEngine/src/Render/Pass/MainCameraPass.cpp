#include "MainCameraPass.hpp"

void MainCameraPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH24STENCIL8, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture->create();
    depth_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->setDepthAttachment(depth_ttachment);
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

//void MainCameraPass::prepare(FrameBuffer* framebuffer)
//{
//    if (framebuffer)
//        m_shadow_map = framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap();
//    else
//        m_shadow_map = 0;
//}

void MainCameraPass::configShader(bool skybox, bool reflection, bool normal_debug, bool wireframe)
{
    //config shader ����
    m_skybox = skybox;
    m_reflection = reflection;
    m_normal_debug = normal_debug;
    m_wireframe = wireframe;
}

void MainCameraPass::configSamples(int samples)
{
    //config FrameBuffer
    m_framebuffer->bind();
    //m_framebuffer->setSamples(samples);
}

void MainCameraPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightReferenceMatrix;
    Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
    Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

    static RenderShaderObject* shader = RenderShaderObject::getShaderObject(Asset::ShaderType::PBRShader);
    shader->start_using();
    int k = 0;
    for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
        std::string light_id = std::string("pointLights[") + std::to_string(k) + "]";
        shader->setFloat3(light_id + ".position", render_point_light_data.position);
        shader->setFloat4(light_id + ".color", render_point_light_data.color);
        shader->setFloat(light_id + ".radius", render_point_light_data.radius);
        k++;
    }
    shader->setInt("point_lights_size", k);
    for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
        const auto& render_sub_mesh_data = pair.second;
        auto& material = render_sub_mesh_data->renderMaterialData();
        // temp
        shader->setFloat3("albedo", material.albedo);
        shader->setFloat("metallic", material.metallic);
        shader->setFloat("roughness", material.roughness);
        shader->setFloat("ao", material.ao);

        shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        shader->setMatrix("view", 1, m_render_source_data->view_matrix);
        shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
        shader->setFloat3("cameraPos", m_render_source_data->camera_position);

        shader->setFloat3("directionalLight.direction", light_direction);
        shader->setFloat4("directionalLight.color", light_color);

        shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
        shader->setBool("enable_skybox_sample", m_reflection);
        if (m_shadow_map != 0) {
            shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
            shader->setTexture("shadow_map", 5, m_shadow_map);

            for (int i = 0; i < m_cube_maps.size(); i++) {
                std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
                shader->setCubeTexture(cube_map_id, 6 + i, m_cube_maps[i]);
            }
        }
        m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
    shader->stop_using();
    
    static RenderShaderObject* skybox_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::SkyboxShader);
    if (m_skybox) {
        const auto& render_skybox_sub_mesh_data = m_render_source_data->render_skybox_data.render_sub_mesh_data;
        auto& material = render_skybox_sub_mesh_data->renderMaterialData();
        skybox_shader->start_using();
        skybox_shader->setMatrix("model", 1, render_skybox_sub_mesh_data->transform());
        skybox_shader->setMatrix("view", 1, Mat4(Mat3(m_render_source_data->view_matrix)));
        skybox_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
        skybox_shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
        m_rhi->drawIndexed(render_skybox_sub_mesh_data->getVAO(), render_skybox_sub_mesh_data->indicesCount());
        skybox_shader->stop_using();
    }
}
