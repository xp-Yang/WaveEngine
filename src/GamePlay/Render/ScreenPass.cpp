#include "ScreenPass.hpp"
#include "GamePlay/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Application.hpp"

void ScreenPass::init()
{
	m_default_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_default_framebuffer->createDefault();
	// ÓÃÀ´downSampleµÄ
	m_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGBA });
	m_screen_quad = new ScreenQuad();
	m_screen_quad->create();
}

void ScreenPass::prepare(FrameBuffer* framebuffer)
{
	framebuffer->blitColorMapTo(m_framebuffer);
}

void ScreenPass::draw()
{
	m_default_framebuffer->bind();
	m_default_framebuffer->clear();
	auto main_viewport = Application::GetApp().getWindow()->getMainViewport();
	glViewport(main_viewport.x, main_viewport.y, main_viewport.width, main_viewport.height);

	glDisable(GL_DEPTH_TEST);
	static Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap());
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
	glEnable(GL_DEPTH_TEST);


	// a child window for debugging
	glViewport(main_viewport.width - main_viewport.width / 4, main_viewport.y, main_viewport.width / 4, main_viewport.height / 4);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, 66);
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
}

FrameBuffer* ScreenPass::getFrameBuffer()
{
	return m_framebuffer;
}
