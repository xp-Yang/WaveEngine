#include "Window.hpp"
#include <assert.h>
#include <utility>

Window::Window(int width, int height)
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

	m_main_viewport.x = WINDOW_WIDTH - MAIN_VIEWPORT_WIDTH;
	m_main_viewport.y = WINDOW_HEIGHT - MAIN_VIEWPORT_HEIGHT;
	m_main_viewport.width = MAIN_VIEWPORT_WIDTH;
	m_main_viewport.height = MAIN_VIEWPORT_HEIGHT;
	glfwSetWindowUserPointer(m_window, &m_main_viewport);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		Viewport& main_viewport = *(Viewport*)glfwGetWindowUserPointer(window);
		float scale_factor = std::min((float)width / (float)main_viewport.width, (float)height / (float)main_viewport.height);
		main_viewport.width *= scale_factor;
		main_viewport.height *= scale_factor;
		main_viewport.x = 0;
		main_viewport.y = height - main_viewport.height;
		glViewport(main_viewport.x, main_viewport.y, main_viewport.width, main_viewport.height);
		});
}

void Window::shutdown()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

void Window::update() const
{
	glfwPollEvents();//检查触发事件（键盘输入、鼠标移动等）
	glfwSwapBuffers(m_window);//交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），输出在屏幕上。
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
