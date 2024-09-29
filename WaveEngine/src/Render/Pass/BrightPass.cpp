#include "BrightPass.hpp"

BrightPass::BrightPass()
{
}

void BrightPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

void BrightPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    auto lighted_map = m_input_passes[0]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
    static RenderShaderObject* extract_bright_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::ExtractBrightShader);
    extract_bright_shader->start_using();
    extract_bright_shader->setTexture("Texture", 0, lighted_map);
    m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
    extract_bright_shader->stop_using();
}
