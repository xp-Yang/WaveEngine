#include "InputSystem.hpp"
#include <imgui/imgui.h>
#include "Core/Math.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include <WaveEngine/Application.hpp>

void InputSystem::refreshState()
{
	ImGuiIO& io = ImGui::GetIO();

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

	m_mouse_x = (float)io.MousePos.x;
	m_mouse_y = (float)io.MousePos.y;
}

void InputSystem::onUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantPassThroughMouse) {
		// ImGui级别的鼠标输入，移动窗口等。
		return;
	}

	refreshState();

	if (m_last_mouse_state == MouseState::Holding && m_mouse_state == MouseState::Released) {
		pick();
	}

	m_camera_system.onUpdate();

	if (m_mouse_state == MouseState::Dragging) {
		float delta_x = m_mouse_x - m_last_mouse_x;
		float delta_y = -(m_mouse_y - m_last_mouse_y);
		m_last_mouse_x = m_mouse_x;
		m_last_mouse_y = m_mouse_y;
		m_camera_system.onMouseUpdate(delta_x, delta_y, m_mouse_button);
	}

	if (!isApproxZero(io.MouseWheel))
		m_camera_system.onMouseWheelUpdate(io.MouseWheel, m_mouse_x, m_mouse_y);

	float avrg_frame_time = 1.0f / io.Framerate;
	if (io.KeyShift)
		avrg_frame_time /= 10.0f;
	if (io.KeysDown['W']) {
		m_camera_system.onKeyUpdate('W', avrg_frame_time);
	}
	if (io.KeysDown['A']) {
		m_camera_system.onKeyUpdate('A', avrg_frame_time);
	}
	if (io.KeysDown['S']) {
		m_camera_system.onKeyUpdate('S', avrg_frame_time);
	}
	if (io.KeysDown['D']) {
		m_camera_system.onKeyUpdate('D', avrg_frame_time);
	}

	if (m_last_mouse_state != m_mouse_state) {
		m_last_mouse_x = m_mouse_x;
		m_last_mouse_y = m_mouse_y;
		m_last_mouse_state = m_mouse_state;
	}
}

void InputSystem::pick()
{
	// TODO 用事件处理
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	auto& world = ecs::World::get();
	if (ImGui::GetIO().MouseReleased[0]) {
		//glFlush();
		//glFinish();
		//// TODO: 含义？
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// TODO 尝试 glGetTextureImage 
		glBindFramebuffer(GL_FRAMEBUFFER, 6);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		int x = (int)ImGui::GetIO().MousePos.x;
		int y = (int)ImGui::GetIO().MousePos.y;
		// map to picking framebuffer size
		auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport()).transToScreenCoordinates();
		x -= main_viewport.x;
		y -= main_viewport.y;
		x *= WINDOW_WIDTH / main_viewport.width;
		y *= WINDOW_HEIGHT / main_viewport.height;
		// glReadPixels()的坐标是相对于屏幕左下角的
		y = WINDOW_HEIGHT - y;

		unsigned char data[4] = { 0,0,0,0 };
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);

		for (const auto& picked_entity : world.getPickedEntities()) {
			world.removeComponent<ecs::PickedComponent>(picked_entity);
		}
		for (auto entity : world.entityView<ecs::RenderableComponent>()) {
			if (entity.getId() * 50000 == picked_id) {
				world.addComponent<ecs::PickedComponent>(entity);
			}
		}
		//glReadBuffer(GL_NONE);
	}
}
