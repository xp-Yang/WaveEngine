#include "Window.hpp"
#include "Platform/OpenGL/rhi_opengl.hpp"
#include <GLFW/glfw3.h>
#include <assert.h>

void Window::create(int width, int height)
{
	glfwInit();//初始化GLFW; glfwWindowHint()配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3,这样GLFW会在创建OpenGL上下文时做出适当的调整
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//明确告诉GLFW我们使用的是核心模式(Core-profile)
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//不允许用户调整窗口的大小
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 16);

	m_window = glfwCreateWindow(width, height, "RenderEngine", nullptr, nullptr);
	if (m_window == nullptr)
	{
		assert(false);
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window);

	//glfwSetWindowUserPointer(m_window, &m_Data);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		//glfwGetWindowUserPointer(window);
		;// glViewport(0, 0, width, height);
		});
}

void Window::shutdown()
{
	glfwDestroyWindow(m_window);
}

void Window::setMainViewport(const Viewport& viewport)
{
	m_main_viewport = viewport;
	glViewport(m_main_viewport.x, m_main_viewport.y, m_main_viewport.width, m_main_viewport.height);
}

GLFWwindow* Window::getNativeWindowHandle() const
{
	return m_window;
}

int Window::getWidth() const
{
	return m_width;
}

int Window::getHeight() const
{
	return m_height;
}

Viewport Window::getMainViewport() const
{
	return m_main_viewport;
}
