#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "../../imgui/imgui.h"
#include "../View.hpp"

class ImGuiEditor {
public:
	ImGuiEditor(const View& view);
	void render();
	void render_global_editor();
	void render_camera_editor();
	void render_entity_editor();

	bool pixel_style;
	bool stop_rotate;
	bool normal_debug;
	float ambient_strength;
	int icosphere_accuracy;

private:
	const View& view;
};

#endif