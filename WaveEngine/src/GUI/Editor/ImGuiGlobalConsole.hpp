#ifndef ImGuiGlobalConsole_hpp
#define ImGuiGlobalConsole_hpp

struct RenderDashBoardParams { // TODO
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
class ImGuiGlobalConsole {
public:
	ImGuiGlobalConsole();
	void render();

protected:
	void updateRenderParams();

private:
	RenderSystem* m_ref_render_system;
	RenderDashBoardParams m_render_params;
};

#endif