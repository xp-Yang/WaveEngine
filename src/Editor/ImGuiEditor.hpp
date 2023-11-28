#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "GamePlay/Render/RenderSystem.hpp"
#include "GamePlay/Framework/ECS/MotionSystem.hpp"

class ImGuiEditor {
public:
	ImGuiEditor();
	void init(RenderSystem* render_system, MotionSystem* motion_system);
	void render();
	void render_global_editor();
	void render_camera_editor();
	void render_entity_editor();
	void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
	void update_render_params();

	bool stop_rotate;
	float global_ambient_strength;
	int icosphere_accuracy;

private:
	// 无所有权的，不 out live 所指向的实际资源
	RenderSystem* ref_render_system;
	MotionSystem* ref_motion_system;
	RenderParams m_render_params;
};

#endif