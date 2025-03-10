#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "GUI/Viewport.hpp"

class MainCanvas;
class PreviewCanvas;
class ImGuiContextMenu;
class ImGuiSceneHierarchy;
class ImGuiGlobalConsole;
class ImGuiDebugWindow;
class ImGuiEditor {
public:
	ImGuiEditor();
	~ImGuiEditor();
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
};

#endif