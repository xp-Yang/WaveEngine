#ifndef ImGuiContextMenu_hpp
#define ImGuiContextMenu_hpp

class ImGuiContextMenu {
public:
	void render();
	void popUp();
	void dismiss();

protected:
	bool m_menu_opened{ false };
};

#endif