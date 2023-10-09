#pragma once

// �ӿ���
// ��������
// �ϲ�ֻ����FrameBuffer*������Ҫ���Ĵ�����ʵ���Ǿ���OpenGLFrameBuffer����VulkanFrameBuffer

// TODO ʵ��opengl���͵�RHI����Ҫ����Щͷ�ļ���¶���ϲ㣬�ϲ�ֻ֪��RHI�ӿڣ�
#include <glad/glad.h>
#include "OpenGL/FrameBuffer.hpp"
#include "OpenGL/Renderer.hpp"
#include "OpenGL/Shader.hpp"

class RHI {
public:
	virtual ~RHI() = 0;
	virtual void drawIndex() = 0;
	virtual void drawTriangles() = 0;
	virtual void createFrameBuffer() = 0;
	virtual void setViewport() = 0;
};