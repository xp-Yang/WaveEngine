#include "Application.hpp"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "GamePlay/ECS/Components.hpp"
#include "GamePlay/ECS/RenderSystem.hpp"
#include "GamePlay/ECS/MotionSystem.hpp"
#include <windows.h>

#define PERFORMANCE_TEST

void App::run() {
	create_multisample_fbo();
	create_depth_fbo();
	//create_post_processing_fbo();
	create_screen_fbo();
	//create_picking_fbo();

	unsigned int quad_VAO = FrameBufferQuad().get_quad_VAO();

	while (!glfwWindowShouldClose(m_window)) {
#ifdef PERFORMANCE_TEST
		LARGE_INTEGER t1, t2, tc;
		QueryPerformanceFrequency(&tc);
		QueryPerformanceCounter(&t1);
#endif

		new_frame();

		// input System
		m_view.mouse_and_key_callback();

		// motion System
		ecs::MotionSystem::onUpdate();

		//logic.onUpdate();

		//ecs::RenderSystem::onUpdate();

		// 1. ���������ͼ
		float depth_buffer_width = WINDOW_WIDTH * 16;
		float depth_buffer_height = WINDOW_HEIGHT * 16;
		glBindTexture(GL_TEXTURE_2D, depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_buffer_width, depth_buffer_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glViewport(0, 0, depth_buffer_width, depth_buffer_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		Renderer::render_shadow_map();
			// debug depth
			//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glBindTexture(GL_TEXTURE_2D, view.get_shadow_map_id());
			//renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);

		// 2. ����msaa��ɫ��ͼ
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		ecs::RenderSystem::onUpdate();
		// ������ȾҲ��msaa
		if (m_editor.normal_debug)
			Renderer::render_normal();

		// 3.���ɺ�����ͼ
		if (m_editor.pixel_style) {

		}
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisample_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen_fbo);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		// ��blit�Ļ���ֱ��ʹ�� sampler2DMS �Զ��忹����㷨

		// 4. Ĭ�ϻ���
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		frame_shader->setTexture("Texture", 0, screen_texture);
		Renderer::drawTriangle(*frame_shader, quad_VAO, 6);
		glEnable(GL_DEPTH_TEST);

		// render imgui
		m_editor.render();

		end_frame();

#ifdef PERFORMANCE_TEST
		QueryPerformanceCounter(&t2);
		auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
		std::cout << "gameloop time: " << time << std::endl;
#endif
	}
}

void App::init()
{
	create_window((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS ģʽ

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	m_scene.init();
	m_view.set_scene(&m_scene);

	frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
}

void App::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void App::new_frame()
{
	glfwPollEvents();//��鴥���¼����������롢����ƶ��ȣ�

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void App::end_frame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);//������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩���������Ļ�ϡ�
}

void App::create_window(int size_x, int size_y) {
	//glfwInit��������ʼ��GLFW��glfwWindowHint����������GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3,����GLFW���ڴ���OpenGL������ʱ�����ʵ��ĵ���
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//��ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//���Ҳ������û��������ڵĴ�С
	//ֻ��Ĭ��FBO��Ч:
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 16);

	m_window = glfwCreateWindow(size_x, size_y, "RenderEngine", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_window);

	//��ʼ��GLAD��ʹ����Թ���OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
}

void App::create_multisample_fbo()
{
	glGenFramebuffers(1, &multisample_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
	int samples_count = 16;
	if(samples_count == 1)
	{
		unsigned int color_texture;
		glGenTextures(1, &color_texture);
		glBindTexture(GL_TEXTURE_2D, color_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
		// ΪʲôҪ������Ȼ������work? tex_buffer�����Ѿ�����Ȳ��Թ���һ��������
		// Ϊʲôֻ����stencil����Ӱ�ͻ������⣿
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		//glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); 
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
	}
	else {
		unsigned int multi_sample_texture;
		glGenTextures(1, &multi_sample_texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multi_sample_texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_count, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multi_sample_texture, 0);
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_count, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void App::create_depth_fbo()
{
	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glGenTextures(1, &depth_texture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
	m_view.set_shadow_map_id(depth_texture);
	//��������ɫ�����֡��������ǲ������ģ�����������Ҫ��ʽ����OpenGL���ǲ������κ���ɫ���ݽ�����Ⱦ��
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::create_post_processing_fbo()
{
	// configure second post-processing framebuffer
}

void App::create_screen_fbo()
{
	glGenFramebuffers(1, &screen_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);
}

void App::create_picking_fbo()
{
	// ����picking֡����
	//unsigned int picking_FBO;
	//glGenFramebuffers(1, &picking_FBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, picking_FBO);
	//view.set_picking_FBO(picking_FBO);
	//unsigned int picking_texture;
	//glActiveTexture(GL_TEXTURE8);
	//glGenTextures(1, &picking_texture);
	//glBindTexture(GL_TEXTURE_2D, picking_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);
	//unsigned int picking_depth_texture;
	//glGenTextures(1, &picking_depth_texture);
	//glActiveTexture(GL_TEXTURE7);
	//glBindTexture(GL_TEXTURE_2D, picking_depth_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, picking_depth_texture, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
