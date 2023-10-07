#include "ScreenPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

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
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glDisable(GL_DEPTH_TEST);
	static Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap());
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
	glEnable(GL_DEPTH_TEST);

	// debug
	//glViewport(WINDOW_WIDTH, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	//frame_shader->start_using();
	//frame_shader->setTexture("Texture", 0, 57);
	//Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
}

FrameBuffer* ScreenPass::getFrameBuffer()
{
	return m_framebuffer;
}
