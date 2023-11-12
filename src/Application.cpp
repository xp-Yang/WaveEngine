#include "Application.hpp"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "GamePlay/Framework/ECS/Components.hpp"
#include "GamePlay/Framework/ECS/MotionSystem.hpp"
#include <windows.h>
#include <iostream>

#define PERFORMANCE_TEST 0

void Application::run() {
	while (!m_window->shouldClose()) {
#if PERFORMANCE_TEST
		LARGE_INTEGER t1, t2, tc;
		QueryPerformanceFrequency(&tc);
		QueryPerformanceCounter(&t1);
#endif
		newFrame(); // automatically handle imgui input

		// input System
		m_input_system->mouse_and_key_callback();

		// motion System
		m_motion_system->onUpdate();

		// render System
		m_render_system->onUpdate();
		// render imgui
		m_editor.render();

		endFrame();
#if PERFORMANCE_TEST
		QueryPerformanceCounter(&t2);
		auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
		std::cout << "gameloop time: " << time << std::endl;
#endif
	}
}

void Application::init()
{
	m_window = std::make_unique<Window>((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);

	//初始化GLAD，使其可以管理OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(false);
	}

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window->getNativeWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	m_scene.init();
	m_render_system = std::make_unique<RenderSystem>();
	m_render_system->initPipeline();
	m_motion_system = std::make_unique<MotionSystem>();
	m_input_system = std::make_unique<InputSystem>();
	m_editor.init(m_render_system.get(), m_motion_system.get());
}

void Application::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	m_window->shutdown();
}

Window* Application::getWindow()
{
	return m_window.get();
}

void Application::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_window->update();
}