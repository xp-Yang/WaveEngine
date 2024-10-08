#include "BloomPass.hpp"

BloomPass::BloomPass()
{
    m_type = RenderPass::Type::Bloom;
}

void BloomPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);

    {
        RhiTexture* color_texture_1 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
        color_texture_1->create();
        RhiAttachment color_attachment_1 = RhiAttachment(color_texture_1);
        RhiFrameBuffer* fb_1 = m_rhi->newFrameBuffer(color_attachment_1, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
        fb_1->create();
        m_pingpong_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb_1);
    }
}

void BloomPass::draw()
{
    extractBright();
    blur();
}

void BloomPass::extractBright()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    auto lighted_map = m_input_passes[0]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
    static RenderShaderObject* extract_bright_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::ExtractBrightShader);
    extract_bright_shader->start_using();
    extract_bright_shader->setTexture("Texture", 0, lighted_map);
    m_rhi->drawIndexed(m_render_source_data->screen_quad->getVAO(), m_render_source_data->screen_quad->indicesCount());
    extract_bright_shader->stop_using();
}

void BloomPass::blur()
{
    m_pingpong_framebuffer->bind();
    m_pingpong_framebuffer->clear();

    auto bright_map = m_framebuffer->colorAttachmentAt(0)->texture()->id();
    unsigned int map = m_pingpong_framebuffer->colorAttachmentAt(0)->texture()->id();

    static RenderShaderObject* blur_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::GaussianBlur);
    bool horizontal = true;
    unsigned int amount = 16;
    blur_shader->start_using();
    for (unsigned int i = 0; i < amount; i++)
    {
        blur_shader->setTexture("image", 0, (i == 0) ? bright_map : map);
        blur_shader->setInt("horizontal", horizontal);
        m_rhi->drawIndexed(m_render_source_data->screen_quad->getVAO(), m_render_source_data->screen_quad->indicesCount());
        horizontal = !horizontal;
    }
    blur_shader->stop_using();

    m_pingpong_framebuffer->blitTo(m_framebuffer.get());
}
