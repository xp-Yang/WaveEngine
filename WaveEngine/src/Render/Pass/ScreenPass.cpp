#include "ScreenPass.hpp"

void ScreenPass::init()
{
	m_default_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_default_framebuffer->createDefault();

	// ÓÃÀ´downSampleµÄ
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGBA });
}

void ScreenPass::prepare(FrameBuffer* framebuffer)
{
	framebuffer->blitColorMapTo(m_framebuffer.get());
	m_scene_map = m_framebuffer->getFirstAttachmentOf(AttachmentType::RGBA).getMap();
}

void ScreenPass::draw()
{
	m_default_framebuffer->bind();
	m_default_framebuffer->clear();

	auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
	Application::GetApp().getWindow()->setMainViewport(main_viewport);
	static RenderShaderObject* frame_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::QuadShader);
	frame_shader->start_using();
	frame_shader->setTexture("Texture", 0, m_scene_map);
	frame_shader->setTexture("bloomBlurMap", 1, m_blurred_bright_map);
	frame_shader->setTexture("borderMap", 2, m_border_map);
	frame_shader->setBool("bloom", true);
	frame_shader->setBool("border", true);
	Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());


	frame_shader->setBool("bloom", false);
	frame_shader->setBool("border", false);
	// child window for debugging
	if (m_pick_view_ref) {
		Viewport picking_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::Pick).value_or(Viewport());
		Application::GetApp().getWindow()->setViewport(ViewportType::Pick, picking_viewport);
		frame_shader->start_using();
		frame_shader->setTexture("Texture", 0, m_pick_view_ref->getFirstAttachmentOf(AttachmentType::RGB16F).getMap());
		Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	}

	if (m_shadow_view_ref) {
		Viewport shadow_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::Shadow).value_or(Viewport());
		Application::GetApp().getWindow()->setViewport(ViewportType::Shadow, shadow_viewport);
		frame_shader->start_using();
		frame_shader->setTexture("Texture", 0, m_shadow_view_ref->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
		Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	}
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

void ScreenPass::setBlurredBrightMap(unsigned int map)
{
	m_blurred_bright_map = map;
}

void ScreenPass::setBorderMap(unsigned int map)
{
	m_border_map = map;
}
