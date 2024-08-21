#ifndef ImGuiSceneHierarchy_hpp
#define ImGuiSceneHierarchy_hpp

#include "Core/Common.hpp"

class GObject;
class Scene;
class ImGuiSceneHierarchy {
public:
	ImGuiSceneHierarchy();
	void init();
	void render();

protected:
	void renderNode(GObject* node);
	void renderLeafNode(Meta::DynamicReflectionInstance& refl_instance);

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_gui_creator;
	Scene* m_ref_scene;
};

#endif