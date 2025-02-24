#ifndef ImGuiGcodeLegend_hpp
#define ImGuiGcodeLegend_hpp

#include "Base/Common.hpp"
#include <imgui.h>
#include <imgui_internal.h>

class ImGuiCanvas;
class GcodeLegend {
public:
	GcodeLegend(ImGuiCanvas* parent);
	void render();

private:
	ImGuiCanvas* m_parent;
};

#endif // !ImGuiGcodeLegend_hpp
