#include "ScreenPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Application.hpp"

void ScreenPass::init()
{
	m_default_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_default_framebuffer->createDefault();
	// 用来downSample的
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGBA });
	m_screen_quad = std::make_unique<ScreenQuad>();
	m_screen_quad->create();
}

void ScreenPass::prepare(FrameBuffer* framebuffer)
{
	framebuffer->blitColorMapTo(m_framebuffer.get());
}

void ScreenPass::draw()
{
	m_default_framebuffer->bind();
	m_default_framebuffer->clear();

	auto main_viewport = Application::GetApp().getWindow()->getMainViewport();
	Application::GetApp().getWindow()->setMainViewport(main_viewport);
	glDisable(GL_DEPTH_TEST);
	static Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap());
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
	glEnable(GL_DEPTH_TEST);


	// TODO 可以塞多个小窗口进多个imgui窗口里
	// a child window for debugging
	//auto main_viewport = Application::GetApp().getWindow()->getMainViewport();
	auto child_viewport = Viewport(main_viewport.x + main_viewport.width - main_viewport.width / 4, main_viewport.y, main_viewport.width / 4, main_viewport.height / 4, Viewport::Coordinates::GLCoordinates);
	Application::GetApp().getWindow()->setMainViewport(child_viewport);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, 66);
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
	Application::GetApp().getWindow()->setMainViewport(main_viewport);
}

FrameBuffer* ScreenPass::getFrameBuffer()
{
	return m_framebuffer.get();
}
