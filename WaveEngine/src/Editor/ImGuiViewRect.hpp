#ifndef ImGuiViewRect_hpp
#define ImGuiViewRect_hpp

#include "GamePlay/FrameWork/ECS/World.hpp"
#include "GamePlay/FrameWork/ECS/Components.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>
#include <vector>

class ViewRect;

class ViewRectManager {
public:
	ViewRectManager();
	~ViewRectManager();
	void render();

private:
	std::vector<ViewRect*> m_views;
};

#endif