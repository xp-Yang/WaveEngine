#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include <imgui/imgui.h>
#include "GamePlay/Render/RenderSystem.hpp"
#include "GamePlay/FrameWork/ECS/World.hpp"

enum class ToolbarType : int {
	Translate,
	Rotate,
	Scale,
};

class ImGuiEditor {
public:
	ImGuiEditor();
	void init(RenderSystem* render_system);
	void render();

	bool motion;
	float global_ambient_strength;
	int icosphere_accuracy;

protected:
	void renderGlobalController();
	void renderSceneHierarchy();
	void renderCameraController();
	void renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities);
	void renderMainViewWindow();
	void renderPickingViewWindow();
	void renderShadowViewWindow();
	void renderRayTracingViewWindow();
	void renderGizmos();
	void updateRenderParams();

private:
	// 无所有权的，不 out live 所指向的实际资源
	RenderSystem* m_ref_render_system;
	RenderParams m_render_params;

	ToolbarType m_toolbar_type{ ToolbarType::Translate };
};

#endif