#ifndef ImGuiContextMenu_hpp
#define ImGuiContextMenu_hpp

#include "Core/Common.hpp"

enum class ContextType {
	Void,
	Object,
	Light,
};

class ImGuiEditor;
class ImGuiContextMenu {
public:
	ImGuiContextMenu(ImGuiEditor* parent) : m_parent(parent) {}
	void render();
	void popUp(ContextType context);

protected:
	bool m_menu_opened{ false };
	ContextType m_context{ ContextType::Void };

	ImGuiEditor* m_parent;
};

#endif