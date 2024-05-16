#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"
#include <memory>

class ImGuiEditor;
class RenderSystem;
class InputSystem;
class Scene;
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
	Scene* getScene();

private:
	Application();
	void newFrame();
	void endFrame();

	std::unique_ptr<Window> m_window;
	
	std::unique_ptr<ImGuiEditor> m_editor;
	std::unique_ptr<RenderSystem> m_render_system;
	std::unique_ptr<InputSystem> m_input_system;
	std::unique_ptr<Scene> m_scene;
};

#endif
