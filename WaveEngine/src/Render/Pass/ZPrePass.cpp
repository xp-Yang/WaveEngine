#include "ZPrePass.hpp"

ZPrePass::ZPrePass()
{
    m_type = RenderPass::Type::ZPre;
}

void ZPrePass::init()
{
    RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    depth_texture->create();
    RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(RhiAttachment(), Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->setDepthAttachment(depth_ttachment);
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

void ZPrePass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* depth_shader = RenderShaderObject::getShaderObject(ShaderType::OneColorShader);
    depth_shader->start_using();
    Mat4 light_view = m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
    Mat4 light_proj = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix;
    for (const auto& pair : m_render_source_data->render_mesh_nodes) {
        const auto& render_node = pair.second;
        if (render_node->material.alpha != 1.0f)
            continue;
        depth_shader->setMatrix("model", 1, render_node->model_matrix);
        depth_shader->setMatrix("view", 1, light_view);
        depth_shader->setMatrix("projection", 1, light_proj);
        depth_shader->setFloat4("color", Color4(1.0));
        m_rhi->drawIndexed(render_node->mesh.getVAO(), render_node->mesh.indicesCount());
    }
}
