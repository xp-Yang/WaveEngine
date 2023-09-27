#include "ScreenPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

void ScreenPass::init()
{
	m_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGB });
	m_screen_quad = new ScreenQuad();
	m_screen_quad->create();
}

void ScreenPass::prepare(FrameBuffer* framebuffer)
{
	framebuffer->blitColorMapTo(m_framebuffer);
}

void ScreenPass::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	static Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
	frame_shader->setTexture("Texture", 0, m_framebuffer->getFirstAttachmentOf(AttachmentType::RGB).getMap());
	Renderer::drawTriangle(*frame_shader, m_screen_quad->getVAO(), 6);
	glEnable(GL_DEPTH_TEST);


	//// debug depth
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//frame_shader->setTexture("Texture", 0, 57); // shadow map
	//Renderer::drawTriangle(*frame_shader, quad_vao, 6);
}

FrameBuffer* ScreenPass::getFrameBuffer()
{
	return m_framebuffer;
}

unsigned int ScreenPass::ScreenQuad::create()
{
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
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	return m_vao;
}

unsigned int ScreenPass::ScreenQuad::getVAO()
{
	return m_vao;
}
