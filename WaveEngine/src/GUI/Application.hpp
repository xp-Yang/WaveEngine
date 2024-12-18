#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"
#include "Logical/FrameWork/World/Scene.hpp"
#include "Render/RenderSystem.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Editor/ImGuiInput.hpp"
#include "Logical/Animation/AnimationSystem.hpp"

static inline const std::string RESOURCE_DIRECTORY = RESOURCE_DIR;
static inline const std::string ASSET_DIRECTORY = ASSET_DIR;

static inline int    FRAMES_PER_SECOND = 60;
static inline double MILLISECONDS_PER_FRAME = 1000. / FRAMES_PER_SECOND;

class Application {
public:
	~Application();
	static Application& get();
	void run();
	void init();
	void shutdown();

private:
	Application();

	std::shared_ptr<Window> m_window;

	std::shared_ptr<ImGuiEditor> m_editor;
	std::shared_ptr<RenderSystem> m_render_system;
	std::shared_ptr<AnimationSystem> m_animation_system;
	std::shared_ptr<GUIInput> m_gui_input;
	std::shared_ptr<Scene> m_scene;
};

#endif
