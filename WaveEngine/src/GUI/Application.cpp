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
	while (!m_window->shouldClose()) {
		// TODO control frame rate
		// logical
		m_input_system->onUpdate();
		m_animation_system->onUpdate();

		// render
		m_render_system->onUpdate();

		// gui
		m_editor->onUpdate();

		m_window->swapBuffer();
	}
}

void Application::init()
{
	Meta::Register::allMetaRegister();

	m_window = std::make_unique<Window>((int)DEFAULT_WINDOW_WIDTH, (int)DEFAULT_WINDOW_HEIGHT);

	m_scene = std::make_unique<Scene>();
	m_scene->init();
	m_input_system = std::make_unique<InputSystem>();
	m_input_system->init();
	m_animation_system = std::make_unique<AnimationSystem>();
	m_render_system = std::make_unique<RenderSystem>();
	m_render_system->init();
	m_editor = std::make_unique<ImGuiEditor>();
}

void Application::shutdown()
{
	m_window->shutdown();
}