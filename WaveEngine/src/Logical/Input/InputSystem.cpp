#include "InputSystem.hpp"
#include "PickingSolver.hpp"
#include <Engine.hpp>

void InputSystem::init()
{
	m_camera_manipulator = std::make_shared<CameraManipulator>();

	connect(this, &picking, &PickingSolver::get(), &PickingSolver::onPicking);
}

bool InputSystem::onUpdate()
{
	{ // TODO 事件触发
		auto gui_input = GetApp().editor()->guiInput();
		m_mouse_button = gui_input->m_mouse_button;
		m_mouse_state = gui_input->m_mouse_state;
		m_mouse_x = gui_input->m_mouse_x;
		m_mouse_y = gui_input->m_mouse_y;
		m_delta_mouse_x = gui_input->m_delta_mouse_x;
		m_delta_mouse_y = gui_input->m_delta_mouse_y;
		m_mouse_wheel = gui_input->m_mouse_wheel;
		m_frame_time = gui_input->m_frame_time;
		memcpy(KeysDown, gui_input->KeysDown, sizeof(KeysDown));
	}

	if (m_last_mouse_state == MouseState::Holding && m_mouse_state == MouseState::Released) {
		int x = m_mouse_x;
		int y = m_mouse_y;
		// map to picking framebuffer size
		// picking framebuffer is full screen size
		auto main_viewport = GetApp().editor()->canvasManager()->getMainViewport();
		x *= DEFAULT_RENDER_RESOLUTION_X / (float)main_viewport.width;
		y *= DEFAULT_RENDER_RESOLUTION_Y / (float)main_viewport.height;
		// glReadPixels()的坐标是相对于屏幕左下角的
		y = DEFAULT_RENDER_RESOLUTION_Y - y;
		Logger::debug("InputSystem::onUpdate()，picking({}, {}), mouse({}, {})", x, y, m_mouse_x, m_mouse_y);
		emit picking(x, y);
	}

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
