#ifndef ImGuiInput_hpp
#define ImGuiInput_hpp

#include "Logical/Input/InputEnums.hpp"
#include "Core/Common.hpp"
#include "Logical/Input/CameraManipulator.hpp"

class GUIInput {
public:
	void init();
	bool onUpdate();

protected:
	bool refreshState();
	Vec2 mapToMainCanvasWindow(const Vec2& value);

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

	bool KeysDown[Key_COUNT];

	float m_frame_time;

	std::shared_ptr<CameraManipulator> m_camera_manipulator;
};

#endif