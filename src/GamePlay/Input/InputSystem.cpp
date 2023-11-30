#include "InputSystem.hpp"
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include "GamePlay/Framework/ECS/Components.hpp"
#include "GamePlay/Framework/ECS/CameraSystem.hpp"

void InputSystem::mouse_and_key_callback()
{
	ImGuiIO& io = ImGui::GetIO();

	float xpos, ypos;
	xpos = (float)io.MousePos.x;
	ypos = (float)io.MousePos.y;
	if ((io.WantCaptureMouse && !io.WantPassThroughMouse) || ImGuizmo::IsUsing()) {
		return;
	}
	static bool last_left_mouse_status = io.MouseDown[0];
	static bool last_right_mouse_status = io.MouseDown[1];

	static float last_pos_x = WINDOW_WIDTH / 2;
	static float last_pos_y = WINDOW_HEIGHT / 2;
	if (last_left_mouse_status != io.MouseDown[0]) {
		last_pos_x = xpos;
		last_pos_y = ypos;
		last_left_mouse_status = io.MouseDown[0];
	}
	if (io.MouseDown[0]) {
		float delta_x = xpos - last_pos_x;
		float delta_y = -(ypos - last_pos_y);
		last_pos_x = xpos;
		last_pos_y = ypos;
		ecs::CameraSystem::onMouseUpdate(delta_x, delta_y, 0);
	}

	if (last_right_mouse_status != io.MouseDown[1]) {
		last_pos_x = xpos;
		last_pos_y = ypos;
		last_right_mouse_status = io.MouseDown[1];
	}
	if (io.MouseDown[1]) {
		float delta_x = xpos - last_pos_x;
		float delta_y = -(ypos - last_pos_y);
		last_pos_x = xpos;
		last_pos_y = ypos;
		ecs::CameraSystem::onMouseUpdate(delta_x, delta_y, 1);
	}

	ecs::CameraSystem::onMouseWheelUpdate(io.MouseWheel);

	float avrg_frame_time = 1.0f / io.Framerate;
	if (io.KeyShift)
		avrg_frame_time /= 10.0f;
	if (io.KeysDown['W']) {
		ecs::CameraSystem::onKeyUpdate('W', avrg_frame_time);
	}
	if (io.KeysDown['A']) {
		ecs::CameraSystem::onKeyUpdate('A', avrg_frame_time);
	}
	if (io.KeysDown['S']) {
		ecs::CameraSystem::onKeyUpdate('S', avrg_frame_time);
	}
	if (io.KeysDown['D']) {
		ecs::CameraSystem::onKeyUpdate('D', avrg_frame_time);
	}

}
