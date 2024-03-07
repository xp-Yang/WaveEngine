#include "Application_impl.hpp"
#include <windows.h>
#include <iostream>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/ImGuizmo.h>
#include "GamePlay/Framework/SceneHierarchy.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "GamePlay/Render/RenderSystem.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "GamePlay/Framework/ECS/MotionSystem.hpp"
#include "GamePlay/Input/InputSystem.hpp"

#define PERFORMANCE_TEST 0

Application::~Application() = default;

Application& Application::GetApp()
{
	static Application app;
	return app;
}

std::string Application::resourceDirectory()
{
	return RESOURCE_DIR;
}

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

		// render
		m_editor->render();

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
		assert(false);

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	ImGui_ImplGlfw_InitForOpenGL(m_window->getNativeWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	m_scene_hierarchy = std::make_unique<SceneHierarchy>();
	m_render_system = std::make_unique<RenderSystem>();
	m_motion_system = std::make_unique<MotionSystem>();
	m_input_system = std::make_unique<InputSystem>();
	m_editor = std::make_unique<ImGuiEditor>();
	m_editor->init(m_render_system.get());
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

SceneHierarchy* Application::getSceneHierarchy()
{
	return m_scene_hierarchy.get();
}

void Application::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
}

void Application::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	m_window->update();
}