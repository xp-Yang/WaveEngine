#include "Window.hpp"
#include <assert.h>
#include <utility>

float WINDOW_WIDTH(1920.0f);
float WINDOW_HEIGHT(1080.0f);

Window::Window(int width, int height)
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
	m_main_viewport.width = WINDOW_WIDTH;
	m_main_viewport.height = WINDOW_HEIGHT;
	glfwSetWindowUserPointer(m_window, &m_main_viewport);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		if (width <= 0 || height <= 0)
			return;

		float scale_factor = std::min((float)width / WINDOW_WIDTH, (float)height / WINDOW_HEIGHT);
		WINDOW_WIDTH = width;
		WINDOW_HEIGHT = height;

		//Viewport& main_viewport = *(Viewport*)glfwGetWindowUserPointer(window);
		//main_viewport.transToScreenCoordinates();
		//main_viewport.width *= scale_factor;
		//main_viewport.height *= scale_factor;
		//main_viewport.transToGLCoordinates();
		//glViewport(main_viewport.x, main_viewport.y, main_viewport.width, main_viewport.height);
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
	glfwPollEvents();//��鴥���¼����������롢����ƶ��ȣ�
	glfwSwapBuffers(m_window);//������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩���������Ļ�ϡ�
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
