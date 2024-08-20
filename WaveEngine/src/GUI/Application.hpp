#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"
#include "Logical/FrameWork/Scene.hpp"
#include "Render/RenderSystem.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Logical/Input/InputSystem.hpp"
#include "Logical/Animation/AnimationSystem.hpp"

class Application {
public:
	~Application();
	static Application& get();
	void run();
	void init();
	void shutdown();
	Window* window() const { return m_window.get();	}
	Scene* scene() const { return m_scene.get(); }
	RenderSystem* renderSystem() const { return m_render_system.get(); }
	ImGuiEditor* editor() const { return m_editor.get(); }

private:
	Application();

	std::unique_ptr<Window> m_window;

	std::unique_ptr<ImGuiEditor> m_editor;
	std::unique_ptr<RenderSystem> m_render_system;
	std::unique_ptr<AnimationSystem> m_animation_system;
	std::unique_ptr<InputSystem> m_input_system;
	std::unique_ptr<Scene> m_scene;
};

#endif
