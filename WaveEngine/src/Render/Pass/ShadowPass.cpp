#include "ShadowPass.hpp"
// TODO remove
#include <glad/glad.h>
void ShadowPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture->create();
    depth_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->setDepthAttachment(depth_ttachment);
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);

    size_t max_point_light_count = Asset::maxPointLightCount;
    reinit_cube_maps(max_point_light_count);

    glGenFramebuffers(1, &m_cube_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowPass::configSamples(int samples)
{
    return;
    //TODO
    //config FrameBuffer
    //m_framebuffer->bind();
    //m_framebuffer->setSamples(samples);

    //glBindTexture(GL_TEXTURE_2D, m_framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, samples * m_framebuffer->getWidth(), samples * m_framebuffer->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void ShadowPass::draw() {
    drawDirectionalLightShadowMap();
    drawPointLightShadowMap();
}

void ShadowPass::clear()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_framebuffer->unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    for(const auto& cube_map : m_cube_maps)
        for (int i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cube_map, 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::drawDirectionalLightShadowMap()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* depth_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::DepthShader);
    depth_shader->start_using();
    Mat4 light_view = m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
    Mat4 light_proj = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix;
    for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
        const auto& render_sub_mesh_data = pair.second;
        depth_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        depth_shader->setMatrix("view", 1, light_view);
        depth_shader->setMatrix("projection", 1, light_proj);
        m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
}

void ShadowPass::drawPointLightShadowMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glViewport(0, 0, DEFAULT_RENDER_RESOLUTION_Y, DEFAULT_RENDER_RESOLUTION_Y); // TODO 显然窗口大小变化后不是default大小

    static RenderShaderObject* depth_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::CubeMapShader);

    depth_shader->start_using();

    std::vector<std::array<Mat4, 6>> light_view;
    std::vector<Mat4> light_proj;
    std::vector<Vec3> light_pos;
    std::vector<float> light_radius;
    for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
        light_view.push_back(render_point_light_data.lightViewMatrix);
        light_proj.push_back(render_point_light_data.lightProjMatrix);
        light_pos.push_back(render_point_light_data.position);
        light_radius.push_back(render_point_light_data.radius);
    }

    if (m_render_source_data->render_point_light_data_list.size() > m_cube_maps.size()) {
        reinit_cube_maps(m_render_source_data->render_point_light_data_list.size());
    }

    for (int cube_map_id = 0; cube_map_id < m_render_source_data->render_point_light_data_list.size(); cube_map_id++) {
        for (int i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cube_maps[cube_map_id], 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
                const auto& render_sub_mesh_data = pair.second;
                depth_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
                depth_shader->setMatrix("view", 1, light_view[cube_map_id][i]);
                depth_shader->setMatrix("projection", 1, light_proj[cube_map_id]);
                depth_shader->setFloat3("lightPos", light_pos[cube_map_id]);
                depth_shader->setFloat("far_plane", light_radius[cube_map_id]);
                m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
            }
        }
    }
}

void ShadowPass::reinit_cube_maps(size_t count)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);

    m_cube_maps.assign(count, 0);
    for (int i = 0; i < m_cube_maps.size(); i++) {
        glGenTextures(1, &m_cube_maps[i]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_maps[i]);
        for (GLuint i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, DEFAULT_RENDER_RESOLUTION_Y, DEFAULT_RENDER_RESOLUTION_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}
