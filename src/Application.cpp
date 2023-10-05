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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS ģʽ

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
	glfwPollEvents();//��鴥���¼����������롢����ƶ��ȣ�

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::end_frame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);//������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩���������Ļ�ϡ�
}

void Application::create_window(int size_x, int size_y) {
	//glfwInit��������ʼ��GLFW��glfwWindowHint����������GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3,����GLFW���ڴ���OpenGL������ʱ�����ʵ��ĵ���
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//��ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//���Ҳ������û��������ڵĴ�С
	//ֻ��Ĭ��FBO��Ч:
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

	//��ʼ��GLAD��ʹ����Թ���OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(false);
		return;
	}
}