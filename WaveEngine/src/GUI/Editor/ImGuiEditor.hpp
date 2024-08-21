#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "Core/Common.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "ImGuiInput.hpp"
#include "ImGuiGlobalConsole.hpp"
#include "ImGuiSceneHierarchy.hpp"
#include "ImGuiDebugWindow.hpp"

class ImGuiEditor {
public:
	ImGuiEditor();
	~ImGuiEditor();
	void init();
	void onUpdate();
	ImGuiViewPortWindowManager* viewPortWindowManager() const { return m_view_manager.get(); }
	GUIInput* guiInput() const { return m_gui_input.get(); }

protected:
	void renderGlobalMenu();
	void renderEmptyMainDockerSpaceWindow();
#if ENABLE_ECS
	void renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities);
#endif
	void configUIStyle();

private:
	std::unique_ptr<ImGuiViewPortWindowManager> m_view_manager;
	std::unique_ptr<GUIInput> m_gui_input;
	std::unique_ptr<ImGuiSceneHierarchy> m_scene_hierarchy_window;
	std::unique_ptr<ImGuiGlobalConsole> m_global_console_window;
	std::unique_ptr<ImGuiDebugWindow> m_debug_window;
};

#endif