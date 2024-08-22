#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "Core/Common.hpp"
#include "ImGuiInput.hpp"
#include "ImGuiCanvas.hpp"
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
	GUIInput* guiInput() const { return m_gui_input.get(); }

protected:
	void renderMenuBar();
	void renderEmptyMainDockerSpaceWindow();
	void configUIStyle();

private:
	std::unique_ptr<GUIInput> m_gui_input;
	std::unique_ptr<ImGuiCanvasManager> m_canvas_manager;
	std::unique_ptr<ImGuiSceneHierarchy> m_scene_hierarchy_window;
	std::unique_ptr<ImGuiGlobalConsole> m_global_console_window;
	std::unique_ptr<ImGuiDebugWindow> m_debug_window;
};

#endif