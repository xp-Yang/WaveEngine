#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "InputEnums.hpp"
#include "Logical/Framework/ECS/CameraSystem.hpp"

class InputSystem {
public:
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

	ecs::CameraSystem m_camera_system;
};

#endif // !InputSystem_hpp

