#ifndef Application_hpp
#define Application_hpp

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Renderer.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "GamePlay/ECS/RenderSystem.hpp"

class App {
public:
	static App& GetApp() {
		static App app;
		return app;
	}
	void run();
	void init();
	void shutdown();
	void new_frame();
	void end_frame();
private:
	App() = default;
	void create_window(int size_x, int size_y);

	GLFWwindow* m_window{ nullptr };
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;
	ecs::RenderSystem m_render_system;
};

#endif
