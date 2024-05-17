#include "Application_impl.hpp"

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/ImGuizmo.h>

#include "Logical/Input/InputSystem.hpp"
#include "Render/RenderSystem.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Logical/FrameWork/Scene.hpp"

#include "WaveEngine/Window.hpp"
#include <GLFW/glfw3.h>

#include "AllMetaRegister.hpp"

Application::Application()
{
}

Application::~Application() {};

Application& Application::GetApp()
{
	// reenter problem
	// two steps initialization
	static Application app;
	return app;
}

std::string Application::resourceDirectory()
{
	return RESOURCE_DIR;
}

std::string Application::assetDirectory()
{
	return ASSET_DIR;
}

void Application::run() {
	while (!m_window->shouldClose()) {
		newFrame(); // automatically handle imgui input

		// logical
		// input System
		m_input_system->onUpdate();

		// render
		m_render_system->onUpdate();

		// gui
		m_editor->onUpdate();

		endFrame();
	}
}

void Application::init()
{
	Meta::Register::allMetaRegister();

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

	m_scene = std::make_unique<Scene>();
	m_scene->init();
	m_input_system = std::make_unique<InputSystem>();
	m_input_system->init();
	m_render_system = std::make_unique<RenderSystem>();
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

Scene* Application::getScene()
{
	return m_scene.get();
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

	m_window->swapBuffer();
}