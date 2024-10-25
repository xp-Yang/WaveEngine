#include "Application.hpp"
#include "Core/Common.hpp"
#include "AllMetaRegister.hpp"

Application::Application() {}

Application::~Application() {}

Application& Application::get()
{
	static Application app;
	return app;
}

void Application::run() {
	//Timer fps_timer;
	//fps_timer.start();
	while (!m_window->shouldClose()) {
		//double duration = fps_timer.duration();
		//if (duration >= MILLISECONDS_PER_FRAME) {
			//fps_timer.restart();
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
		//}
	}
	//fps_timer.stop();
}

void Application::init()
{
	Meta::Register::allMetaRegister();

	m_window = std::make_shared<Window>((int)DEFAULT_WINDOW_WIDTH, (int)DEFAULT_WINDOW_HEIGHT);

	m_scene = std::make_shared<Scene>();
	m_scene->init();
	m_animation_system = std::make_shared<AnimationSystem>();
	m_render_system = std::make_shared<RenderSystem>();
	m_render_system->init(m_scene);
	m_editor = std::make_shared<ImGuiEditor>();
	m_editor->init(m_window, m_render_system, m_scene);
	m_gui_input = std::make_shared<GUIInput>();
	m_gui_input->init(m_editor);
}

void Application::shutdown()
{
	m_window->shutdown();
}