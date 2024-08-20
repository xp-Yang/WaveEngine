#include "ScreenPass.hpp"

void ScreenPass::init()
{
	// ÓÃÀ´downSampleµÄ
	RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
	color_texture->create();
	RhiAttachment color_attachment = RhiAttachment(color_texture);
	RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
	fb->create();
	m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

//void ScreenPass::prepare(FrameBuffer* framebuffer)
//{
//	//framebuffer->blitColorMapTo(m_framebuffer.get());
//	//m_lighted_map = m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap();
//	m_lighted_map = framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
//}

void ScreenPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

	static RenderShaderObject* frame_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::QuadShader);
	frame_shader->start_using();
	m_lighted_map = m_input_passes[0]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
	frame_shader->setTexture("Texture", 0, m_lighted_map);
	m_border_map = m_input_passes[1]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
	frame_shader->setTexture("borderMap", 1, m_border_map);
	frame_shader->setBool("border", true);
	//frame_shader->setTexture("bloomBlurMap", 2, m_blurred_bright_map);
	frame_shader->setBool("bloom", false);
	m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());

	m_framebuffer->unBind(); // bind GL_FRAMEBUFFER to default: 0
}
