#ifndef ImGuiSceneHierarchy_hpp
#define ImGuiSceneHierarchy_hpp

#include "Base/Common.hpp"

class GObject;
class Light;
class ImGuiEditor;
class ImGuiSceneHierarchy {
public:
	ImGuiSceneHierarchy(ImGuiEditor* parent);
	void render();

protected:
	void renderNodes(const std::vector<GObject*>& nodes);
	void renderNodes(const std::vector<Light*>& nodes);
	void renderReflectionWidget(Meta::Instance& refl_instance);

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_widget_creator;
	ImGuiEditor* m_parent;
};

#endif