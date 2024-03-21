#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "InputEnums.hpp"

class InputSystem {
public:
	void update();
	void mouse_and_key_callback();

protected:
	void pick();

protected:
	MouseState m_last_mouse_state{ MouseState::None };
	MouseState m_mouse_state{ MouseState::None };
	MouseButton m_mouse_button{ MouseButton::None };

	float m_last_mouse_x;
	float m_last_mouse_y;

	float m_mouse_x;
	float m_mouse_y;
};

#endif // !InputSystem_hpp

