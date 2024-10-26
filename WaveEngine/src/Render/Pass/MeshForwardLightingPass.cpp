#include "MeshForwardLightingPass.hpp"

void MeshForwardLightingPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB8, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y), 4);
    RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH24STENCIL8, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y), 4);
    color_texture->create();
    depth_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y), 4);
    fb->setDepthAttachment(depth_ttachment);
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

void MeshForwardLightingPass::enableReflection(bool reflection)
{
    //config shader ����
    m_reflection = reflection;
}

void MeshForwardLightingPass::enablePBR(bool pbr)
{
    m_pbr = pbr;
}

void MeshForwardLightingPass::configSamples(int samples)
{
    //config FrameBuffer
    m_framebuffer->bind();
    //m_framebuffer->setSamples(samples);
}

void MeshForwardLightingPass::draw()
{
    m_framebuffer->bind();
    //m_framebuffer->clear(Color4(0.046, 0.046, 0.046, 1.0)); // before gamma correction
    m_framebuffer->clear(Color4(0.251, 0.251, 0.251, 1.0)); // after gamma correction

    Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix * 
        m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
    Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
    Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

    static RenderShaderObject* pbr_shader = RenderShaderObject::getShaderObject(ShaderType::PBRShader);
    static RenderShaderObject* blinn_phong_shader = RenderShaderObject::getShaderObject(ShaderType::BlinnPhongShader);
    RenderShaderObject* shader = m_pbr ? pbr_shader : blinn_phong_shader;
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
    for (const auto& pair : m_render_source_data->render_mesh_nodes) {
        const auto& render_node = pair.second;
        auto& material = render_node->material;
        // temp
        if (m_pbr) {
            shader->setFloat3("albedo", material.albedo);
            shader->setFloat("metallic", material.metallic);
            shader->setFloat("roughness", material.roughness);
            shader->setFloat("ao", material.ao);
        }
        else {
            shader->setTexture("material.diffuse_map", 0, material.diffuse_map);
            shader->setTexture("material.specular_map", 1, material.specular_map);
            shader->setTexture("material.normal_map", 2, material.normal_map);
            shader->setTexture("material.height_map", 3, material.height_map);
        }

        shader->setMatrix("model", 1, render_node->model_matrix);
        shader->setMatrix("view", 1, m_render_source_data->view_matrix);
        shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
        shader->setFloat3("cameraPos", m_render_source_data->camera_position);

        shader->setFloat3("directionalLight.direction", light_direction);
        shader->setFloat4("directionalLight.color", light_color);

        if (m_shadow_map != 0) {
            shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
            shader->setTexture("shadow_map", 5, m_shadow_map);

            for (int i = 0; i < m_cube_maps.size(); i++) {
                std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
                shader->setCubeTexture(cube_map_id, 6 + i, m_cube_maps[i]);
            }
        }
        m_rhi->drawIndexed(render_node->mesh.getVAO(), render_node->mesh.indicesCount());
    }
    shader->stop_using();
}
