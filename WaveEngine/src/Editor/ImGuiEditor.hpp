#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "ImGuiViewRect.hpp"
#include "Logical/FrameWork/Scene.hpp"
#include "Render/RenderSystem.hpp"

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

enum class ToolbarType : int {
	Translate,
	Rotate,
	Scale,
};

class ImGuiEditor {
public:
	ImGuiEditor();
	void init(RenderSystem* render_system);
	void onUpdate();

protected:
	void renderGlobalMenu();
	void renderEmptyMainDockerSpaceWindow();
	void renderGlobalConsole();
	void renderCameraController();
	void renderSceneHierarchyNode(GObject* node);
	void renderLeafNode(Meta::DynamicReflectionInstance& refl_instance);
	void renderSceneHierarchy();
	void renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities);

	void renderGizmos();
	void updateRenderParams();

	void configUIStyle();

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_gui_creator;

	std::unique_ptr<ViewRectManager> m_view_manager;
	// 无所有权的，不 out live 所指向的实际资源
	RenderSystem* m_ref_render_system;
	RenderParams m_render_params;

	ToolbarType m_toolbar_type{ ToolbarType::Translate };
};

#endif