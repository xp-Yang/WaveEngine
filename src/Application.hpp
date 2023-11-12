#ifndef Application_hpp
#define Application_hpp

#include "GamePlay/Framework/Scene.hpp"
#include "GamePlay/Input/InputSystem.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Platform/RHI/rhi.hpp"
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
	Window* getWindow();

private:
	Application() = default;
	void newFrame();
	void endFrame();

	std::unique_ptr<Window> m_window;
	Scene m_scene;
	ImGuiEditor m_editor;
	std::unique_ptr<RenderSystem> m_render_system;
	std::unique_ptr<MotionSystem> m_motion_system;
	std::unique_ptr<InputSystem> m_input_system;
};

#endif
