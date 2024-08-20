#ifndef ImGuiEditor_hpp
#define ImGuiEditor_hpp

#include "Core/Common.hpp"
#include "ImGuiImGuiViewPortWindow.hpp"
#include "ImGuiInput.hpp"

enum class ToolbarType : int {
	Translate,
	Rotate,
	Scale,
};

struct RenderDashBoardParams { // TODO gui≤„Œ¨ª§ / render≤„Œ¨ª§
	int     msaa_sample_count = 4;
	bool    skybox = false;
	bool    shadow = true;
	int     shadow_map_sample_count = 1;

	// for MainCameraPass
	bool    reflection = false;
	bool    normal_debug = false;
	bool    wireframe = false;
	bool    checkerboard = false;
	bool    pbr = true;
	//int     pixelate_level = 1;
};

class RenderSystem;
class GObject;
class ImGuiEditor {
public:
	ImGuiEditor();
	~ImGuiEditor();
	void init();
	void onUpdate();
	ImGuiViewPortWindowManager* viewPortWindowManager() const { return m_view_manager.get(); }
	GUIInput* guiInput() const { return m_gui_input.get(); }

protected:
	void renderGlobalMenu();
	void renderEmptyMainDockerSpaceWindow();
	void renderGlobalConsole();
	void renderCameraController();
	void renderSceneHierarchyNode(GObject* node);
	void renderLeafNode(Meta::DynamicReflectionInstance& refl_instance);
	void renderSceneHierarchy();

#if ENABLE_ECS
	void renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities);
#endif

	void renderGizmos();
	void updateRenderParams();

	void configUIStyle();

private:
	std::unordered_map<std::string, std::function<void(std::string, void*)>> m_gui_creator;

	std::unique_ptr<ImGuiViewPortWindowManager> m_view_manager;
	std::unique_ptr<GUIInput> m_gui_input;
	RenderSystem* m_ref_render_system;
	RenderDashBoardParams m_render_params;

	ToolbarType m_toolbar_type{ ToolbarType::Translate };
};

#endif