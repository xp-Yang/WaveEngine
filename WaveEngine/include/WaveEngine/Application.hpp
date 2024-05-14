#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"
#include <memory>

namespace ecs {
	class SceneHierarchy;
	class MotionSystem;
}
class ImGuiEditor;
class RenderSystem;
class InputSystem;
// È«¾ÖÀà
class Application {
public:
	~Application();
	static Application& GetApp();
	static std::string resourceDirectory();
	static std::string assetDirectory();
	void run();
	void init();
	void shutdown();
	Window* getWindow();
	ecs::SceneHierarchy* getSceneHierarchy();

private:
	Application();
	void newFrame();
	void endFrame();

	std::unique_ptr<Window> m_window;
	
	std::unique_ptr<ecs::SceneHierarchy> m_scene_hierarchy;

	std::unique_ptr<ImGuiEditor> m_editor;
	std::unique_ptr<RenderSystem> m_render_system;
	std::unique_ptr<ecs::MotionSystem> m_motion_system;
	std::unique_ptr<InputSystem> m_input_system;
};

#endif
