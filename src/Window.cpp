#include "Window.hpp"
#include "Platform/OpenGL/rhi_opengl.hpp"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <utility>

void Window::create(int width, int height)
{
	glfwInit();//��ʼ��GLFW; glfwWindowHint()����GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3,����GLFW���ڴ���OpenGL������ʱ�����ʵ��ĵ���
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//��ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//�������û��������ڵĴ�С
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 16);

	m_window = glfwCreateWindow(width, height, "RenderEngine", nullptr, nullptr);
	if (m_window == nullptr)
	{
		assert(false);
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window);

	m_main_viewport.x = 0;
	m_main_viewport.y = 0;
	m_main_viewport.width = width;
	m_main_viewport.height = height;
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
