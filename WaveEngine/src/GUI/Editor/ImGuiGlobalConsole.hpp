#ifndef ImGuiGlobalConsole_hpp
#define ImGuiGlobalConsole_hpp

class RenderSystem;
class ImGuiGlobalConsole {
public:
	ImGuiGlobalConsole();
	void render();

private:
	RenderSystem* m_ref_render_system;
};

#endif