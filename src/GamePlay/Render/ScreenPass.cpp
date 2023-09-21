#include "ScreenPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Renderer.hpp"

void ScreenPass::init()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_map);
	glBindTexture(GL_TEXTURE_2D, m_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_map, 0);


	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions          // uv
	-1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
	 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,   1.0f, 1.0f
	};
	unsigned int quadVBO;
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void ScreenPass::prepare_data(unsigned int fbo, unsigned int map)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void ScreenPass::draw()
{
	glDisable(GL_DEPTH_TEST);
	static Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
	frame_shader->setTexture("Texture", 0, m_map);
	Renderer::drawTriangle(*frame_shader, quad_vao, 6);
	glEnable(GL_DEPTH_TEST);
}
