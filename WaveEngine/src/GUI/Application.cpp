#include "Application.hpp"
#include "Core/Common.hpp"
#include "Render/RHI/rhi.hpp"
#include "AllMetaRegister.hpp"
#include "Engine.hpp"

Application::Application() {}

Application::~Application() {}

Application& Application::get()
{
	static Application app;
	return app;
}

void Application::run() {
	Timer fps_timer;
	fps_timer.start();
	while (!m_window->shouldClose()) {
		double duration = fps_timer.duration();
		if (duration >= MILLISECONDS_PER_FRAME) {
			fps_timer.restart();
			m_editor->beginFrame();
			// logical
			m_gui_input->onUpdate();
			m_animation_system->onUpdate();
			// render
			m_render_system->onUpdate();
			// gui
			m_editor->onUpdate();
			m_editor->endFrame();
			m_window->swapBuffer();
		}
	}
	fps_timer.stop();
}

void Application::init()
{
	Meta::Register::allMetaRegister();

	m_window = std::make_unique<Window>((int)DEFAULT_WINDOW_WIDTH, (int)DEFAULT_WINDOW_HEIGHT);

	m_scene = std::make_unique<Scene>();
	m_scene->init();
	m_gui_input = std::make_unique<GUIInput>();
	m_gui_input->init();
	m_animation_system = std::make_unique<AnimationSystem>();
	m_render_system = std::make_unique<RenderSystem>();
	m_render_system->init();
	m_editor = std::make_unique<ImGuiEditor>();
	m_editor->init();
}

void Application::shutdown()
{
	m_window->shutdown();
}