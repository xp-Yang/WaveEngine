#include "ScreenPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Application_impl.hpp"

void ScreenPass::init()
{
	m_default_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_default_framebuffer->createDefault();
	// 用来downSample的
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGBA });
	m_screen_quad = Mesh::create_screen_mesh();
}

void ScreenPass::prepare(FrameBuffer* framebuffer)
{
	framebuffer->blitColorMapTo(m_framebuffer.get());
}

void ScreenPass::draw()
{
	m_default_framebuffer->bind();
	m_default_framebuffer->clear();

	auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
	Application::GetApp().getWindow()->setMainViewport(main_viewport);
	glDisable(GL_DEPTH_TEST);
	Shader* frame_shader = Shader::getShader(ShaderType::QuadShader);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap());
	Renderer::drawIndex(*frame_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
	glEnable(GL_DEPTH_TEST);


	// TODO 可以塞多个小窗口进多个imgui窗口里
	// child window for debugging
	Viewport picking_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::Pick).value_or(Viewport());
	Application::GetApp().getWindow()->setViewport(ViewportType::Pick, picking_viewport);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_pick_view_ref->getFirstAttachmentOf(AttachmentType::RGBA).getMap());
	Renderer::drawIndex(*frame_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());

	Viewport shadow_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::Shadow).value_or(Viewport());
	Application::GetApp().getWindow()->setViewport(ViewportType::Shadow, shadow_viewport);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_shadow_view_ref->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
	Renderer::drawIndex(*frame_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());

	Viewport rt_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::RayTracing).value_or(Viewport());
	Application::GetApp().getWindow()->setViewport(ViewportType::RayTracing, rt_viewport);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_rt_view_ref->getFirstAttachmentOf(AttachmentType::RGB16F).getMap());
	Renderer::drawIndex(*frame_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
}

FrameBuffer* ScreenPass::getFrameBuffer()
{
	return m_framebuffer.get();
}

void ScreenPass::setPickView(FrameBuffer* frame_buffer)
{
	m_pick_view_ref = frame_buffer;
}

void ScreenPass::setShadowView(FrameBuffer* frame_buffer)
{
	m_shadow_view_ref = frame_buffer;
}

void ScreenPass::setRayTracingView(FrameBuffer* frame_buffer)
{
	m_rt_view_ref = frame_buffer;
}
