#ifndef InputSystem_hpp
#define InputSystem_hpp

class InputSystem {
public:
	enum class MouseState {
		None,
		Clicked,
		Holding,
		Dragging,
		Released,
	};

	struct MouseButton {
		enum {
			None = 0,
			Left = 1,
			Right = 1 << 1,
			Middle = 1 << 2,
		};
	};

	void update();
	void mouse_and_key_callback();

protected:
	void pick();

protected:
	MouseState m_last_mouse_state{ MouseState::None };
	MouseState m_mouse_state{ MouseState::None };
	int m_mouse_button{ MouseButton::None };

	float m_last_mouse_x;
	float m_last_mouse_y;

	float m_mouse_x;
	float m_mouse_y;
};

#endif // !InputSystem_hpp

