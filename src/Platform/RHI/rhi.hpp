#pragma once

// �ӿ���
// ��������
// �ϲ�ֻ����FrameBuffer*������Ҫ���Ĵ�����ʵ���Ǿ���OpenGLFrameBuffer����VulkanFrameBuffer

// TODO ʵ��opengl���͵�RHI����Ҫ����Щͷ�ļ���¶���ϲ㣬�ϲ�ֻ֪��RHI�ӿڣ�
#include <glad/glad.h>
#include "OpenGL/FrameBuffer.hpp"
#include "OpenGL/Renderer.hpp"

class RHI {
public:
	virtual ~RHI() = 0;
	// render���
	virtual void drawIndex() = 0;
	virtual void drawTriangles() = 0;
	// buffer
	virtual void createFrameBuffer() = 0;
	// ȫ��״̬
	virtual void setViewport() = 0;
	virtual void enableDepthTest(bool enable) = 0;
	virtual void enableStencilTest(bool enable) = 0;
	virtual void enableBlend(bool enable) = 0;
	// binding resource
	virtual void bindTexture(/*TextureData*/) = 0;
	virtual void bindVertexArray() = 0;
	virtual void bindBuffer(/*BufferData*/) = 0;
};