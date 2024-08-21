#ifndef ImGuiSceneHierarchy_hpp
#define ImGuiSceneHierarchy_hpp

#include "Core/Common.hpp"

class GObject;
class ImGuiSceneHierarchy {
public:
	ImGuiSceneHierarchy() = default;
	void init();
	void render();

protected:
	void renderNode(GObject* node);
	void renderLeafNode(Meta::DynamicReflectionInstance& refl_instance);

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_gui_creator;
};

#endif