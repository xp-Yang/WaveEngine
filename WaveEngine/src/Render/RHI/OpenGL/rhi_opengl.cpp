#include "rhi_opengl.hpp"
#include "OpenGLFrameBuffer.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLBuffer.hpp"
#include "OpenGLVertexLayout.hpp"
#include "OpenGLRenderer.hpp"

RhiOpenGL::RhiOpenGL()
{
	//初始化GLAD，使其可以管理OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		assert(false);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void RhiOpenGL::drawIndexed(unsigned int vao_id, size_t indices_count, int inst_amount)
{
	OpenGLRenderer::drawIndexed(vao_id, indices_count, inst_amount);
}

void RhiOpenGL::drawTriangles(unsigned int vao_id, size_t array_count)
{

}

void RhiOpenGL::setViewport()
{
}

RhiBuffer* RhiOpenGL::newBuffer(RhiBuffer::Type type, RhiBuffer::UsageFlag usage, void* data, int size)
{
	OpenGLBuffer* buffer = new OpenGLBuffer(type, usage, data, size);
	return buffer;
}

RhiVertexLayout* RhiOpenGL::newVertexLayout(RhiBuffer* vbuffer, RhiBuffer* ibuffer)
{
	OpenGLVertexLayout* vertex_layout = new OpenGLVertexLayout(vbuffer, ibuffer);
	return vertex_layout;
}

RhiTexture* RhiOpenGL::newTexture(RhiTexture::Format format, const Vec2& pixelSize, int sampleCount, RhiTexture::Flag flags, unsigned char* data)
{
	OpenGLTexture* texture = new OpenGLTexture(format, pixelSize, sampleCount, flags, data);
	return texture;
}

RhiFrameBuffer* RhiOpenGL::newFrameBuffer(const RhiAttachment& colorAttachment, const Vec2& pixelSize_, int sampleCount_)
{
	OpenGLFrameBuffer* fb = new OpenGLFrameBuffer(colorAttachment, pixelSize_, sampleCount_);
	return fb;
}
