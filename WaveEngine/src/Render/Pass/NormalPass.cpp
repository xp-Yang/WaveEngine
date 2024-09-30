#include "NormalPass.hpp"

NormalPass::NormalPass()
{
    m_type = RenderPass::Type::Normal;
}

void NormalPass::init()
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
}

void NormalPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* normal_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::NormalShader);
    normal_shader->start_using();
    normal_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    normal_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
    normal_shader->setMatrix("projectionView", 1, m_render_source_data->proj_matrix * m_render_source_data->view_matrix);
    for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
        const auto& render_sub_mesh_data = pair.second;
        normal_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
}
