#ifndef Application_hpp
#define Application_hpp

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Renderer.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "FrameBufferQuad.hpp"

class App {
public:
	static App& GetApp() {
		static App app;
		return app;
	}
	void run();
	void init();
	void shutdown();
	void new_frame();
	void end_frame();
private:
	App() = default;
	void create_window(int size_x, int size_y);
	void create_multisample_fbo();
	void create_depth_fbo();
	void create_picking_fbo();
	void create_post_processing_fbo();
	void create_screen_fbo();

	GLFWwindow* m_window{ nullptr };
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;

	unsigned int screen_fbo;
	unsigned int screen_texture;
	unsigned int multisample_fbo;
	unsigned int depth_fbo;
	unsigned int depth_texture;
	unsigned int post_processing_fbo;
	unsigned int picking_fbo;

	Shader* frame_shader{ nullptr };
};

#endif
