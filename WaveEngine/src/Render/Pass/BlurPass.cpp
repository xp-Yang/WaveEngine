#include "BlurPass.hpp"

BlurPass::BlurPass()
{
}

void BlurPass::init()
{
    RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);

    {
        RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
        RhiAttachment color_attachment = RhiAttachment(color_texture);
        color_texture->create();
        RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
        fb->create();
        m_pingpong_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
    }
}

void BlurPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_pingpong_framebuffer->bind();
    m_pingpong_framebuffer->clear();

    m_bright_map = m_input_passes[0]->getFrameBuffer()->colorAttachmentAt(1)->texture()->id();
    unsigned int map1 = m_framebuffer->colorAttachmentAt(0)->texture()->id();
    unsigned int map2 = m_pingpong_framebuffer->colorAttachmentAt(0)->texture()->id();

    static RenderShaderObject* blur_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::GaussianBlur);
    bool horizontal = true;
    unsigned int amount = 32;
    blur_shader->start_using();
    for (unsigned int i = 0; i < amount; i++)
    {
        if (i % 2 == 0) {
            m_framebuffer->bind();
            blur_shader->setTexture("image", 0, (i == 0) ? m_bright_map : map2);
        }
        else {
            m_pingpong_framebuffer->bind();
            blur_shader->setTexture("image", 0, (i == 0) ? m_bright_map : map1);
        }
        blur_shader->setInt("horizontal", horizontal);
        m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
        horizontal = !horizontal;
    }
    blur_shader->stop_using();
}
