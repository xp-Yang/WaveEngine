#ifndef ImGuiGlobalConsole_hpp
#define ImGuiGlobalConsole_hpp

#include "Base/Common.hpp"

class ImGuiEditor;
class ImGuiGlobalConsole {
public:
	ImGuiGlobalConsole(ImGuiEditor* parent) : m_parent(parent) {}
	void render();

private:
	ImGuiEditor* m_parent;
};

#endif