#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include <imgui/imgui.h>
#include "GamePlay/Render/RenderSystem.hpp"

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

	bool stop_rotate;
	float global_ambient_strength;
	int icosphere_accuracy;

protected:
	void renderGlobalController();
	void renderCameraController();
	void renderPickedEntityController();
	void renderMainViewWindow();
	void renderPickingViewWindow();
	void renderShadowViewWindow();
	void renderGizmos();
	void updateRenderParams();

private:
	// 无所有权的，不 out live 所指向的实际资源
	RenderSystem* m_ref_render_system;
	RenderParams m_render_params;

	ToolbarType m_toolbar_type;
};

#endif