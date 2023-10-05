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
	while (!glfwWindowShouldClose(m_window)) {
#if PERFORMANCE_TEST
		LARGE_INTEGER t1, t2, tc;
		QueryPerformanceFrequency(&tc);
		QueryPerformanceCounter(&t1);
#endif
		new_frame(); // automatically handle imgui input

		// input System
		m_view.mouse_and_key_callback();

		// motion System
		m_motion_system->onUpdate();

		// render System
		m_render_system->onUpdate();
		// render imgui
		m_editor.render();

		end_frame();
#if PERFORMANCE_TEST
		QueryPerformanceCounter(&t2);
		auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
		std::cout << "gameloop time: " << time << std::endl;
#endif
	}
}

void Application::init()
{
	create_window((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS 模式

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
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
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::new_frame()
{
	glfwPollEvents();//检查触发事件（键盘输入、鼠标移动等）

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::end_frame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);//交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），输出在屏幕上。
}

void Application::create_window(int size_x, int size_y) {
	//glfwInit函数来初始化GLFW，glfwWindowHint函数来配置GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3,这样GLFW会在创建OpenGL上下文时做出适当的调整
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//明确告诉GLFW我们使用的是核心模式(Core-profile)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//并且不允许用户调整窗口的大小
	//只对默认FBO有效:
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 16);

	m_window = glfwCreateWindow(size_x, size_y, "RenderEngine", nullptr, nullptr);
	if (m_window == nullptr)
	{
		assert(false);
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_window);

	//初始化GLAD，使其可以管理OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(false);
		return;
	}
}