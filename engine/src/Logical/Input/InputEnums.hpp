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

enum Key : int
{
    // Keyboard
    Key_None = 0,
    Key_Tab = 512,             // == Key_NamedKey_BEGIN
    Key_LeftArrow,
    Key_RightArrow,
    Key_UpArrow,
    Key_DownArrow,
    Key_PageUp,
    Key_PageDown,
    Key_Home,
    Key_End,
    Key_Insert,
    Key_Delete,
    Key_Backspace,
    Key_Space,
    Key_Enter,
    Key_Escape,
    Key_LeftCtrl, Key_LeftShift, Key_LeftAlt, Key_LeftSuper,
    Key_RightCtrl, Key_RightShift, Key_RightAlt, Key_RightSuper,
    Key_Menu,
    Key_0, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6, Key_7, Key_8, Key_9,
    Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, Key_H, Key_I, Key_J,
    Key_K, Key_L, Key_M, Key_N, Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T,
    Key_U, Key_V, Key_W, Key_X, Key_Y, Key_Z,
    Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6,
    Key_F7, Key_F8, Key_F9, Key_F10, Key_F11, Key_F12,
    Key_F13, Key_F14, Key_F15, Key_F16, Key_F17, Key_F18,
    Key_F19, Key_F20, Key_F21, Key_F22, Key_F23, Key_F24,
    Key_Apostrophe,        // '
    Key_Comma,             // ,
    Key_Minus,             // -
    Key_Period,            // .
    Key_Slash,             // /
    Key_Semicolon,         // ;
    Key_Equal,             // =
    Key_LeftBracket,       // [
    Key_Backslash,         // \ (this text inhibit multiline comment caused by backslash)
    Key_RightBracket,      // ]
    Key_GraveAccent,       // `
    Key_CapsLock,
    Key_ScrollLock,
    Key_NumLock,
    Key_PrintScreen,
    Key_Pause,
    Key_Keypad0, Key_Keypad1, Key_Keypad2, Key_Keypad3, Key_Keypad4,
    Key_Keypad5, Key_Keypad6, Key_Keypad7, Key_Keypad8, Key_Keypad9,
    Key_KeypadDecimal,
    Key_KeypadDivide,
    Key_KeypadMultiply,
    Key_KeypadSubtract,
    Key_KeypadAdd,
    Key_KeypadEnter,
    Key_KeypadEqual,
    Key_AppBack,               // Available on some keyboard/mouses. Often referred as "Browser Back"
    Key_AppForward,

    // Gamepad (some of those are analog values, 0.0f to 1.0f)                          // NAVIGATION ACTION
    // (download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets)
    Key_GamepadStart,          // Menu (Xbox)      + (Switch)   Start/Options (PS)
    Key_GamepadBack,           // View (Xbox)      - (Switch)   Share (PS)
    Key_GamepadFaceLeft,       // X (Xbox)         Y (Switch)   Square (PS)        // Tap: Toggle Menu. Hold: Windowing mode (Focus/Move/Resize windows)
    Key_GamepadFaceRight,      // B (Xbox)         A (Switch)   Circle (PS)        // Cancel / Close / Exit
    Key_GamepadFaceUp,         // Y (Xbox)         X (Switch)   Triangle (PS)      // Text Input / On-screen Keyboard
    Key_GamepadFaceDown,       // A (Xbox)         B (Switch)   Cross (PS)         // Activate / Open / Toggle / Tweak
    Key_GamepadDpadLeft,       // D-pad Left                                       // Move / Tweak / Resize Window (in Windowing mode)
    Key_GamepadDpadRight,      // D-pad Right                                      // Move / Tweak / Resize Window (in Windowing mode)
    Key_GamepadDpadUp,         // D-pad Up                                         // Move / Tweak / Resize Window (in Windowing mode)
    Key_GamepadDpadDown,       // D-pad Down                                       // Move / Tweak / Resize Window (in Windowing mode)
    Key_GamepadL1,             // L Bumper (Xbox)  L (Switch)   L1 (PS)            // Tweak Slower / Focus Previous (in Windowing mode)
    Key_GamepadR1,             // R Bumper (Xbox)  R (Switch)   R1 (PS)            // Tweak Faster / Focus Next (in Windowing mode)
    Key_GamepadL2,             // L Trig. (Xbox)   ZL (Switch)  L2 (PS) [Analog]
    Key_GamepadR2,             // R Trig. (Xbox)   ZR (Switch)  R2 (PS) [Analog]
    Key_GamepadL3,             // L Stick (Xbox)   L3 (Switch)  L3 (PS)
    Key_GamepadR3,             // R Stick (Xbox)   R3 (Switch)  R3 (PS)
    Key_GamepadLStickLeft,     // [Analog]                                         // Move Window (in Windowing mode)
    Key_GamepadLStickRight,    // [Analog]                                         // Move Window (in Windowing mode)
    Key_GamepadLStickUp,       // [Analog]                                         // Move Window (in Windowing mode)
    Key_GamepadLStickDown,     // [Analog]                                         // Move Window (in Windowing mode)
    Key_GamepadRStickLeft,     // [Analog]
    Key_GamepadRStickRight,    // [Analog]
    Key_GamepadRStickUp,       // [Analog]
    Key_GamepadRStickDown,     // [Analog]

    // Aliases: Mouse Buttons (auto-submitted from AddMouseButtonEvent() calls)
    // - This is mirroring the data also written to io.MouseDown[], io.MouseWheel, in a format allowing them to be accessed via standard key API.
    Key_MouseLeft, Key_MouseRight, Key_MouseMiddle, Key_MouseX1, Key_MouseX2, Key_MouseWheelX, Key_MouseWheelY,

    // [Internal] Reserved for mod storage
    Key_ReservedForModCtrl, Key_ReservedForModShift, Key_ReservedForModAlt, Key_ReservedForModSuper,
    Key_COUNT,
};

#endif