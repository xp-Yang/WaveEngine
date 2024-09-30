#ifndef ImGuiDebugWindow_hpp
#define ImGuiDebugWindow_hpp

#include "Core/Common.hpp"

class ImGuiCanvas;
class ImGuiEditor;
class ImGuiDebugWindow {
public:
	ImGuiDebugWindow(ImGuiEditor* parent);
	void render();

protected:
	std::unique_ptr<ImGuiCanvas> m_picking_canvas;
	std::unique_ptr<ImGuiCanvas> m_shadow_canvas;
	std::unique_ptr<ImGuiCanvas> m_gbuffer_canvas;
	std::unique_ptr<ImGuiCanvas> m_lighting_canvas;
	std::unique_ptr<ImGuiCanvas> m_bright_canvas;
	std::unique_ptr<ImGuiCanvas> m_blurred_canvas;

protected:
	ImGuiEditor* m_parent;
};

#endif