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
	while (!g_context.window->shouldClose()) {
		//double duration = fps_timer.duration();
		//if (duration >= MILLISECONDS_PER_FRAME) {
			//fps_timer.restart();
			g_context.gui_editor->beginFrame();
			// logical
			g_context.gui_input->onUpdate();
			m_animation_system->onUpdate();
			// render
			g_context.render_system->onUpdate(g_context.scene);
			// gui
			g_context.gui_editor->onUpdate();
			g_context.gui_editor->endFrame();
			g_context.window->swapBuffer();
		//}
	}
	//fps_timer.stop();
}

void Application::init()
{
	Meta::Register::allMetaRegister();

	m_animation_system = std::make_shared<AnimationSystem>();
}

void Application::shutdown()
{
	g_context.window->shutdown();
}