#include "Application.hpp"

void App::run() {
	FrameBufferQuad quad;
	unsigned int quad_VAO = quad.get_quad_VAO();
	Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");

	// ����֡����
	unsigned int multiSampleFBO;
	glGenFramebuffers(1, &multiSampleFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, multiSampleFBO);
	//unsigned int color_texture;
	//glGenTextures(1, &color_texture);
	//glBindTexture(GL_TEXTURE_2D, color_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
	int samples_count = 16;
	unsigned int multi_sample_texture;
	glGenTextures(1, &multi_sample_texture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multi_sample_texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_count, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multi_sample_texture, 0);
	// ΪʲôҪ������Ȼ������work? tex_buffer�����Ѿ�����Ȳ��Թ���һ��������
	// Ϊʲôֻ����stencil����Ӱ�ͻ������⣿
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_count, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	//glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); 
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// configure second post-processing framebuffer
	unsigned int postFBO;
	glGenFramebuffers(1, &postFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		assert(false);
	}

	//������Ȼ��壨��Ӱ��
	unsigned int depth_FBO;
	glGenFramebuffers(1, &depth_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
	unsigned int depth_texture;
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
	//��������ɫ�����֡��������ǲ������ģ�����������Ҫ��ʽ����OpenGL���ǲ������κ���ɫ���ݽ�����Ⱦ��
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(m_window))
	{
		m_view.enable_shadow_map(m_editor.enable_shadow);

		// 1. ���������ͼ
		if (m_view.is_shadow_map_enable()) {
			float depth_buffer_width = WINDOW_WIDTH * 16;
			float depth_buffer_height = WINDOW_HEIGHT * 16;
			glBindTexture(GL_TEXTURE_2D, depth_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_buffer_width, depth_buffer_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glViewport(0, 0, depth_buffer_width, depth_buffer_height);
			glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			m_renderer.render_shadow_map(m_view);
			m_view.set_shadow_map_id(depth_texture);

			// debug depth
			//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glBindTexture(GL_TEXTURE_2D, view.get_shadow_map_id());
			//renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);
		}

		// 2. ����msaa��ɫ��ͼ
		float color_buffer_width = WINDOW_WIDTH;
		float color_buffer_height = WINDOW_HEIGHT;
		if (m_editor.pixel_style) {
			color_buffer_width /= 6.0f;
			color_buffer_height /= 6.0f;
		}
		//glBindTexture(GL_TEXTURE_2D, color_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_buffer_width, color_buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glViewport(0, 0, color_buffer_width, color_buffer_height);
		glBindFramebuffer(GL_FRAMEBUFFER, multiSampleFBO);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		m_renderer.render_ecs(m_view);

		if (m_editor.normal_debug)
			m_renderer.render_normal(m_view);

		// 3.���ɺ�����ͼ
		//��Ϊ���ز���������һ���ر����ǲ���ֱ�ӽ����ǵĻ���ͼ�������������㣬��������ɫ���ж����ǽ��в�����
		//һ�����ز�����ͼ���������ͨͼ��������Ϣ��������Ҫ��������С���߻�ԭ(Resolve)ͼ��
		//now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multiSampleFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFBO);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		// ��blit�Ļ���ֱ��ʹ�� sampler2DMS �Զ��忹����㷨

		// 3. Ĭ�ϻ���
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		frame_shader->setTexture("Texture", 0, screenTexture);
		//frame_shader->setTexture("Texture", 0, multi_sample_texture);
		m_renderer.drawTriangle(*frame_shader, quad_VAO, 6);
		glEnable(GL_DEPTH_TEST);

		// render imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		m_view.mouse_and_key_callback();
		m_editor.render();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();//��鴥���¼����������롢����ƶ��ȣ�
		glfwSwapBuffers(m_window);//������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩���������Ļ�ϡ�
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
}

void App::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
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