#ifndef Application_hpp
#define Application_hpp

#include "Application.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MyModel.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "stb_image.h"
#include "ECS/World.hpp"
#include "ECS/Components.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Logger.hpp"
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
private:
	App() = default;
	void create_window(int size_x, int size_y);
	GLFWwindow* m_window{ nullptr };
	Scene m_scene;
	View m_view;
	ImGuiEditor m_editor;
	Renderer m_renderer;
};

#endif
