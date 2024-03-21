#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"
#include <memory>

class SceneHierarchy;
class ImGuiEditor;
class RenderSystem;
class MotionSystem;
class InputSystem;
// È«¾ÖÀà
class Application {
public:
	~Application();
	static Application& GetApp();
	static std::string resourceDirectory();
	void run();
	void init();
	void shutdown();
	Window* getWindow();
	SceneHierarchy* getSceneHierarchy();

private:
	Application();
	void newFrame();
	void endFrame();

	std::unique_ptr<Window> m_window;
	
	std::unique_ptr<SceneHierarchy> m_scene_hierarchy;

	std::unique_ptr<ImGuiEditor> m_editor;
	std::unique_ptr<RenderSystem> m_render_system;
	std::unique_ptr<MotionSystem> m_motion_system;
	std::unique_ptr<InputSystem> m_input_system;
};

#endif
