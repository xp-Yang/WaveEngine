#ifndef InputEnums_hpp
#define InputEnums_hpp

enum class MouseState {
	None,
	Moving,
	Clicked,
	Holding,
	Dragging,
	Released,
};

enum class MouseButton {
	None = 0,
	Left = 1,
	Right = 1 << 1,
	Middle = 1 << 2,
};

#endif