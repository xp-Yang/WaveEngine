#ifndef Application_hpp
#define Application_hpp

#include "View.hpp"
#include "Scene.hpp"
#include "Editor/ImGuiEditor.hpp"

struct GLFWwindow;
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
	GLFWwindow* create_window(int size_x, int size_y);
	void new_frame();
	void end_frame();

	GLFWwindow* m_window{ nullptr };
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;
	std::shared_ptr<ecs::RenderSystem> m_render_system;
	std::shared_ptr<ecs::MotionSystem> m_motion_system;
};

#endif
