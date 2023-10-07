#include "Platform/OpenGL/rhi_opengl.hpp"
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "GamePlay/ECS/Components.hpp"
#include "GamePlay/ECS/RenderSystem.hpp"
#include "GamePlay/ECS/MotionSystem.hpp"
#include <windows.h>
#include <iostream>

#define PERFORMANCE_TEST 0

void Application::run() {
	while (!glfwWindowShouldClose(m_window->getNativeWindowHandle())) {
#if PERFORMANCE_TEST
		LARGE_INTEGER t1, t2, tc;
		QueryPerformanceFrequency(&tc);
		QueryPerformanceCounter(&t1);
#endif
		newFrame(); // automatically handle imgui input

		// input System
		m_view.mouse_and_key_callback();

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
	m_window = std::make_shared<Window>();
	m_window->create((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);

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
	m_view.set_scene(&m_scene);
	m_render_system = std::make_shared<ecs::RenderSystem>();
	m_render_system->initPipeline();
	m_motion_system = std::make_shared<ecs::MotionSystem>();
	m_editor.init(m_render_system, m_motion_system);
}

void Application::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	m_window->shutdown();
	glfwTerminate();
}

std::shared_ptr<Window> Application::getWindow()
{
	return m_window;
}

void Application::newFrame()
{
	glfwPollEvents();//检查触发事件（键盘输入、鼠标移动等）

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window->getNativeWindowHandle());//交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），输出在屏幕上。
}