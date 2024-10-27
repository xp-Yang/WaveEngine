#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "Core/Common.hpp"
#include "ImGuiCanvas.hpp"
#include "ImGuiContextMenu.hpp"
#include "ImGuiSceneHierarchy.hpp"
#include "ImGuiGlobalConsole.hpp"
#include "ImGuiDebugWindow.hpp"

class Window;
class RenderSystem;
class Scene;
class ImGuiEditor {
public:
	ImGuiEditor();
	~ImGuiEditor();
	void init(std::shared_ptr<Window> window, std::shared_ptr<RenderSystem> render_system, std::shared_ptr<Scene> scene);
	void onUpdate();
	void beginFrame();
	void endFrame();
	Viewport getMainViewport() const;
	void popUpMenu();
	bool isInMainCanvas() const;

protected:
	void renderMenuBar();
	void renderEmptyMainDockerSpaceWindow();
	void configUIStyle();

private:
	std::unique_ptr<MainCanvas> m_main_canvas;
	std::unique_ptr<PreviewCanvas> m_preview_canvas;
	std::unique_ptr<ImGuiContextMenu> m_context_menu;
	std::unique_ptr<ImGuiSceneHierarchy> m_scene_hierarchy_window;
	std::unique_ptr<ImGuiGlobalConsole> m_global_console_window;
	std::unique_ptr<ImGuiDebugWindow> m_debug_window;

	bool m_show_debug{ false };

public:
	std::shared_ptr<Window> ref_window;
	std::shared_ptr<RenderSystem> ref_render_system;
	std::shared_ptr<Scene> ref_scene;
};

#endif