#include "RenderPass.hpp"
#include "../../Platform/RHI/rhi.hpp"

unsigned int ScreenQuad::create()
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

unsigned int ScreenQuad::getVAO()
{
	return m_vao;
}
