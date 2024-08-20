#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "InputEnums.hpp"
#include "CameraManipulator.hpp"
#include "Core/Common.hpp"

class InputSystem {
public:
	void init();
	bool onUpdate();

signals:
	Signal<float, float> picking;

protected:
	MouseState m_last_mouse_state{ MouseState::None };
	MouseState m_mouse_state{ MouseState::None };
	MouseButton m_mouse_button{ MouseButton::None };
	float m_mouse_x;
	float m_mouse_y;
	float m_delta_mouse_x;
	float m_delta_mouse_y;
	float m_mouse_wheel;

	bool KeysDown[512];

	float m_frame_time;

	std::shared_ptr<CameraManipulator> m_camera_manipulator;
};

#endif // !InputSystem_hpp

