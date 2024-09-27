#include "ScreenPass.hpp"
// TODO remove
#include <glad/glad.h>
void ScreenPass::init()
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

//void ScreenPass::prepare(FrameBuffer* framebuffer)
//{
	// ÓÃÀ´downSampleµÄ
//	//framebuffer->blitColorMapTo(m_framebuffer.get());
//	//m_lighted_map = m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap();
//	m_lighted_map = framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
//}

void ScreenPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

	// post processing
	static RenderShaderObject* frame_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::QuadShader);
	frame_shader->start_using();
	m_lighted_map = m_input_passes[0]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
	frame_shader->setTexture("Texture", 0, m_lighted_map);
	if (m_input_passes.size() > 1) { // TODO frame graph
		m_border_map = m_input_passes[1]->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
		frame_shader->setTexture("borderMap", 1, m_border_map);
		frame_shader->setBool("border", true);
	}
	else
		frame_shader->setBool("border", false);
	//frame_shader->setTexture("bloomBlurMap", 2, m_blurred_bright_map);
	frame_shader->setBool("bloom", false);
	m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());

	// pristine grid
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_input_passes[0]->getFrameBuffer()->blitTo(m_framebuffer.get(), RhiTexture::Format::DEPTH);
	static RenderShaderObject* grid_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::PristineGridShader);
	grid_shader->start_using();
	grid_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	grid_shader->setMatrix("proj", 1, m_render_source_data->proj_matrix);
	m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	grid_shader->stop_using();

	m_framebuffer->unBind(); // bind GL_FRAMEBUFFER to default: 0
	glDisable(GL_BLEND);
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
