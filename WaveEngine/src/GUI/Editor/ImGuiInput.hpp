#ifndef ImGuiInput_hpp
#define ImGuiInput_hpp

#include "Logical/Input/InputEnums.hpp"
#include "Core/Common.hpp"

class GUIInput {
public:
	void refreshState();

protected:
	Vec2 mapToMainViewportWindow(const Vec2 value);

	friend class InputSystem;

	MouseState m_last_mouse_state{ MouseState::None };
	MouseState m_mouse_state{ MouseState::None };
	MouseButton m_mouse_button{ MouseButton::None };
	float m_last_mouse_x;
	float m_last_mouse_y;
	float m_mouse_x;
	float m_mouse_y;
	float m_delta_mouse_x;
	float m_delta_mouse_y;
	float m_mouse_wheel;

	bool KeysDown[512];

	float m_frame_time;
};

#endif