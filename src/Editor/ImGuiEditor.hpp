#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "GamePlay/ECS/RenderSystem.hpp"
#include "GamePlay/ECS/MotionSystem.hpp"

class ImGuiEditor {
public:
	ImGuiEditor();
	void init(std::shared_ptr<ecs::RenderSystem> render_system, std::shared_ptr<ecs::MotionSystem> motion_system);
	void render();
	void render_global_editor();
	void render_camera_editor();
	void render_entity_editor();
	void update_render_params();

	bool stop_rotate;
	float global_ambient_strength;
	int icosphere_accuracy;

private:
	std::shared_ptr<ecs::RenderSystem> ref_render_system;
	std::shared_ptr<ecs::MotionSystem> ref_motion_system;
	RenderParams m_render_params;
};

#endif