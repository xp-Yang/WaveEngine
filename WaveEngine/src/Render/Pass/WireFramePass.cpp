#include "WireFramePass.hpp"

WireFramePass::WireFramePass()
{
    m_type = RenderPass::Type::WireFrame;
}

void WireFramePass::init()
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

void WireFramePass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* wireframe_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::WireframeShader);
    wireframe_shader->start_using();
    wireframe_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    wireframe_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
    for (const auto& pair : m_render_source_data->render_mesh_nodes) {
        const auto& render_node = pair.second;
        wireframe_shader->setMatrix("model", 1, render_node->model_matrix);
        m_rhi->drawIndexed(render_node->mesh.getVAO(), render_node->mesh.indicesCount());
    }
}
