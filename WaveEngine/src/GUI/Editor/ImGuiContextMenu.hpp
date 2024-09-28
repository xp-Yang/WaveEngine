#ifndef ImGuiContextMenu_hpp
#define ImGuiContextMenu_hpp

enum class ContextType {
	Void,
	Object,
};

class ImGuiContextMenu {
public:
	void render();
	void popUp(ContextType context);
	void dismiss();

protected:
	bool m_menu_opened{ false };
	ContextType m_context{ ContextType::Void };
};

#endif