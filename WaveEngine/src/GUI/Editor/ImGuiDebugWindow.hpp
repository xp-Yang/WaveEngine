#ifndef ImGuiDebugWindow_hpp
#define ImGuiDebugWindow_hpp

#include "Core/Common.hpp"

class ImGuiCanvas;
class ImGuiDebugWindow {
public:
	ImGuiDebugWindow();
	void render();
protected:
	std::unique_ptr<ImGuiCanvas> m_picking_canvas;
	std::unique_ptr<ImGuiCanvas> m_shadow_canvas;
	std::unique_ptr<ImGuiCanvas> m_gbuffer_canvas;
	std::unique_ptr<ImGuiCanvas> m_lighting_canvas;
};

#endif