#pragma once

// 接口类
// 工厂方法
// 上层只持有FrameBuffer*，不需要关心创建的实例是具体OpenGLFrameBuffer还是VulkanFrameBuffer

// TODO 实现opengl类型的RHI，不要把这些头文件暴露给上层，上层只知道RHI接口：
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