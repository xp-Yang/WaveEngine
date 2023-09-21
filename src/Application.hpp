#ifndef Application_hpp
#define Application_hpp

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "View.hpp"
#include "Scene.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "GamePlay/ECS/RenderSystem.hpp"
#include "GamePlay/ECS/MotionSystem.hpp"

class Application {
public:
	static Application& GetApp() {
		static Application app;
		return app;
	}
	void run();
	void init();
	void shutdown();

private:
	Application() = default;
	void create_window(int size_x, int size_y);
	void new_frame();
	void end_frame();

	GLFWwindow* m_window{ nullptr };
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;
	ecs::RenderSystem m_render_system;
	ecs::MotionSystem m_motion_system;
};

#endif
