#ifndef ImGuiSceneHierarchy_hpp
#define ImGuiSceneHierarchy_hpp

#include "Core/Common.hpp"

class GObject;
class Light;
class Scene;
class ImGuiSceneHierarchy {
public:
	ImGuiSceneHierarchy();
	void init();
	void render();

protected:
	void renderNodes(const std::vector<GObject*>& nodes);
	void renderNodes(const std::vector<Light*>& nodes);
	void renderReflectionWidget(Meta::DynamicReflectionInstance& refl_instance);

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_widget_creator;
	Scene* m_ref_scene;
};

#endif