#ifndef Application_hpp
#define Application_hpp

#include "View.hpp"
#include "Scene.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Window.hpp"

class Application {
public:
	static Application& GetApp() {
		static Application app;
		return app;
	}
	void run();
	void init();
	void shutdown();
	std::shared_ptr<Window> getWindow();

private:
	Application() = default;
	void newFrame();
	void endFrame();

	std::shared_ptr<Window> m_window;
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;
	std::shared_ptr<ecs::RenderSystem> m_render_system;
	std::shared_ptr<ecs::MotionSystem> m_motion_system;
};

#endif
