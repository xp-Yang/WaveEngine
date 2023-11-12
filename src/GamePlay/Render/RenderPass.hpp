#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>
#include <memory>
#include "Platform/RHI/rhi.hpp"

// ½Ó¿ÚÀà
class RenderPass {
public:
	RenderPass() = default;
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	virtual ~RenderPass() = default;
	virtual void init() = 0;
	virtual void prepare(FrameBuffer* framebuffer) = 0;
	virtual void draw() = 0;
	virtual FrameBuffer* getFrameBuffer() = 0;
protected:
	std::unique_ptr<FrameBuffer> m_framebuffer;
};

class ScreenQuad {
public:
	unsigned int create() {
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
	unsigned int getVAO() { return m_vao; }
private:
    unsigned int m_vao = 0;
};

#endif // !RenderPass_hpp

