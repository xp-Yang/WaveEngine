#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "Core/Common.hpp"
#include "ImGuiCanvas.hpp"
#include "ImGuiContextMenu.hpp"
#include "ImGuiSceneHierarchy.hpp"
#include "ImGuiGlobalConsole.hpp"
#include "ImGuiDebugWindow.hpp"

class ImGuiEditor {
public:
	ImGuiEditor();
	~ImGuiEditor();
	void init();
	void onUpdate();
	ImGuiCanvasManager* canvasManager() const { return m_canvas_manager.get(); }
	void popUpMenu();
	void dismissMenu();

protected:
	void renderMenuBar();
	void renderEmptyMainDockerSpaceWindow();
	void configUIStyle();

private:
	std::unique_ptr<ImGuiCanvasManager> m_canvas_manager;
	std::unique_ptr<ImGuiContextMenu> m_context_menu;
	std::unique_ptr<ImGuiSceneHierarchy> m_scene_hierarchy_window;
	std::unique_ptr<ImGuiGlobalConsole> m_global_console_window;
	std::unique_ptr<ImGuiDebugWindow> m_debug_window;
};

#endif