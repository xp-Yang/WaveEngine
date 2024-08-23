#include "ImGuiInput.hpp"
#include <imgui.h>
#include "Engine.hpp"

void GUIInput::refreshState()
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantPassThroughMouse) {
		// ImGui级别的鼠标输入：移动窗口等。
		return;
	}

	m_mouse_button = MouseButton::None;
	if (io.MouseDown[0] || ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		m_mouse_button = MouseButton::Left;
	}
	else if (io.MouseDown[1] || ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		m_mouse_button = MouseButton::Right;
	}
	else if (io.MouseDown[2] || ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
		m_mouse_button = MouseButton::Middle;
	}

	m_mouse_state = MouseState::Moving;
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) ||
		ImGui::IsMouseDragging(ImGuiMouseButton_Right) ||
		ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Dragging;
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
		ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
		ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Clicked;
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) ||
		ImGui::IsMouseDown(ImGuiMouseButton_Right) ||
		ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Holding;
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) ||
		ImGui::IsMouseReleased(ImGuiMouseButton_Right) ||
		ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Released;
	}

	m_mouse_x = mapToMainCanvasWindow(Vec2(io.MousePos.x, io.MousePos.y)).x;
	m_mouse_y = mapToMainCanvasWindow(Vec2(io.MousePos.x, io.MousePos.y)).y;
	m_delta_mouse_x = m_mouse_x - m_last_mouse_x;
	m_delta_mouse_y = -(m_mouse_y - m_last_mouse_y);
	m_last_mouse_x = m_mouse_x;
	m_last_mouse_y = m_mouse_y;
	m_mouse_wheel = io.MouseWheel;
	m_frame_time = 1.0f / io.Framerate;
	memcpy(KeysDown, io.KeysDown, sizeof(KeysDown));
	//Logger::debug("delta_x: {}, delta_y: {}", m_delta_mouse_x, m_delta_mouse_y);
}

Vec2 GUIInput::mapToMainCanvasWindow(const Vec2 value)
{
	Vec2 pos = value;
	auto main_viewport = GetApp().editor()->canvasManager()->getMainViewport();
	pos.x -= main_viewport.x;
	pos.y -= main_viewport.y;
	pos.x *= GetApp().window()->getWidth() / main_viewport.width;
	pos.y *= GetApp().window()->getHeight() / main_viewport.height;
	return pos;
}
