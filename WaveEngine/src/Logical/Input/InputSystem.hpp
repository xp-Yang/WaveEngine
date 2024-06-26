#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "InputEnums.hpp"
#include "CameraManipulator.hpp"

class InputSystem {
public:
	void init();
	bool onUpdate();

protected:
	void refreshState();

	void pick();

protected:
	MouseState m_last_mouse_state{ MouseState::None };
	MouseState m_mouse_state{ MouseState::None };
	MouseButton m_mouse_button{ MouseButton::None };

	float m_last_mouse_x;
	float m_last_mouse_y;

	float m_mouse_x;
	float m_mouse_y;

	float m_delta_mouse_x;
	float m_delta_mouse_y;

	std::shared_ptr<CameraManipulator> m_camera_manipulator;
};

#endif // !InputSystem_hpp

