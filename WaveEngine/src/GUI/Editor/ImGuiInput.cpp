#include "ImGuiInput.hpp"
#include "ImGuiEditor.hpp"
#include "Logical/Input/PickingSolver.hpp"
#include "Logical/FrameWork/Scene.hpp"
#include "GUI/Window.hpp"

#include <imgui.h>

void GUIInput::init(std::shared_ptr<ImGuiEditor> editor)
{
	ref_editor = editor;
	m_camera_manipulator = std::make_shared<CameraManipulator>(ref_editor->ref_scene->getMainCamera());
}

bool GUIInput::refreshState()
{
	ImGuiIO& io = ImGui::GetIO();

	auto main_viewport = ref_editor->getMainViewport();
	if (!(main_viewport.x <= io.MousePos.x && io.MousePos.x <= main_viewport.x + main_viewport.width &&
		main_viewport.y <= io.MousePos.y && io.MousePos.y <= main_viewport.y + main_viewport.height) ||
		io.WantCaptureMouse)
		return false;

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
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
		ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
		ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Clicked;
	}
	else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) ||
		ImGui::IsMouseDragging(ImGuiMouseButton_Right) ||
		ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		m_mouse_state = MouseState::Dragging;
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
	return true;
}

bool GUIInput::onUpdate()
{
	if (!refreshState()) {
		ref_editor->dismissMenu();
		return false;
	}

	if (m_mouse_state == MouseState::Clicked)
		ref_editor->dismissMenu();

	if (m_last_mouse_state == MouseState::Holding && m_mouse_state == MouseState::Released) {
		PickingSolver::get().onPicking(m_mouse_x, m_mouse_y, KeysDown[Key_LeftCtrl]);
		if (m_mouse_button == MouseButton::Right) {
			ref_editor->popUpMenu();
		}
	}

	m_camera_manipulator->syncContext(ref_editor->getMainViewport());
	m_camera_manipulator->onUpdate();

	if (m_mouse_state == MouseState::Dragging) {
		m_camera_manipulator->onMouseUpdate(m_delta_mouse_x, m_delta_mouse_y, m_mouse_button);
	}

	if (!isApproxZero(m_mouse_wheel))
		m_camera_manipulator->onMouseWheelUpdate(m_mouse_wheel, m_mouse_x, m_mouse_y);

	if (KeysDown['W']) {
		m_camera_manipulator->onKeyUpdate('W', m_frame_time);
	}
	if (KeysDown['A']) {
		m_camera_manipulator->onKeyUpdate('A', m_frame_time);
	}
	if (KeysDown['S']) {
		m_camera_manipulator->onKeyUpdate('S', m_frame_time);
	}
	if (KeysDown['D']) {
		m_camera_manipulator->onKeyUpdate('D', m_frame_time);
	}

	m_last_mouse_state = m_mouse_state;

	return true;
}

Vec2 GUIInput::mapToMainCanvasWindow(const Vec2& value)
{
	Vec2 pos = value;
	auto main_viewport = ref_editor->getMainViewport();
	pos.x -= main_viewport.x;
	pos.y -= main_viewport.y;
	pos.x *= ref_editor->ref_window->getWidth() / main_viewport.width;
	pos.y *= ref_editor->ref_window->getHeight() / main_viewport.height;
	return pos;
}
