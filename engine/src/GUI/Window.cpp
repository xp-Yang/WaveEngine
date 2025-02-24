#include "GUI/Window.hpp"

#include <GLFW/glfw3.h>
#include "GUI/Platform/Windows/WindowsFileDialog.hpp"

#include <assert.h>
#include <utility>

static void drop_file_callback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; i++) {
		//std::string filepath = paths[i];
		//getSceneHierarchy()->loadModel(filepath);
	}
}

Window::Window(int width, int height)
	: m_width(width)
	, m_height(height)
{
	glfwInit();//��ʼ��GLFW;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//�������û��������ڵĴ�С
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_SAMPLES, 16);
#if NO_TITLE_BAR
	// ���� offscreen context �ı�־λ�����ر�����
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif

	m_window = glfwCreateWindow(width, height, "WaveEngine", nullptr, nullptr);
	if (m_window == nullptr)
	{
		assert(false);
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(0); // disable vsync

	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		if (width <= 0 || height <= 0)
			return;
		Window* thiz = (Window*)glfwGetWindowUserPointer(window);
		thiz->m_width = width;
		thiz->m_height = height;
		});

	glfwSetDropCallback(m_window, drop_file_callback);
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

void Window::swapBuffer() const
{
	glfwPollEvents();//��鴥���¼����������롢����ƶ��ȣ�
	glfwSwapBuffers(m_window);//������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩���������Ļ�ϡ�
}

void* Window::getNativeWindowHandle() const
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

float Window::getAspectRatio() const
{
	return (float)m_width / (float)m_height;
}

FileDialog* Window::createFileDialog()
{
	WindowsFileDialog* file_dialog = new WindowsFileDialog(this);
#ifdef __LINUX__
	auto file_dialog;
#endif
	return file_dialog;
}