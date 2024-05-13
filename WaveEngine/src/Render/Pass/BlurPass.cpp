#include "BlurPass.hpp"

BlurPass::BlurPass()
{
}

void BlurPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F });

    m_pingpong_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_pingpong_framebuffer->create({ AttachmentType::RGB16F });
}

void BlurPass::prepare(FrameBuffer* framebuffer)
{
}

void BlurPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_pingpong_framebuffer->bind();
    m_pingpong_framebuffer->clear();

    unsigned int map1 = m_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
    unsigned int map2 = m_pingpong_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();

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
        Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
        horizontal = !horizontal;
    }
    blur_shader->stop_using();
}

FrameBuffer* BlurPass::getFrameBuffer()
{
    return m_framebuffer.get();
}

unsigned int BlurPass::getBlurredBrightMap()
{
    return m_pingpong_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
}

void BlurPass::setBrightMap(unsigned int map)
{
    m_bright_map = map;
}
