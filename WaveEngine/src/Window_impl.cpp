#include "Window_impl.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <utility>

float WINDOW_WIDTH(1920.0f);
float WINDOW_HEIGHT(1080.0f);
float ASPECT_RATIO(16.0f / 9.0f);

static void drop_file_callback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; i++) {
		std::string filepath = paths[i];
		//Application::GetApp().getSceneHierarchy()->loadModel(filepath);
	}
}

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

	m_viewports[ViewportType::Main] = Viewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Viewport::Coordinates::GLCoordinates);
	glfwSetWindowUserPointer(m_window, &m_viewports[ViewportType::Main]);

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
		//TODO ��װ��rhi
		//glViewport(main_viewport.x, main_viewport.y, main_viewport.width, main_viewport.height);
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

void Window::setMainViewport(const Viewport& viewport)
{
	m_viewports[ViewportType::Main] = viewport;
	// TODO ��װ��rhi
	glViewport(m_viewports[ViewportType::Main].x, m_viewports[ViewportType::Main].y, m_viewports[ViewportType::Main].width, m_viewports[ViewportType::Main].height);
}

void Window::setViewport(ViewportType id, const Viewport& viewport)
{
	m_viewports[id] = viewport;
	// TODO ��װ��rhi
	glViewport(m_viewports[id].x, m_viewports[id].y, m_viewports[id].width, m_viewports[id].height);
}

std::optional<Viewport> Window::getViewport(ViewportType id) const
{
	if (m_viewports.find(id) != m_viewports.end())
		return m_viewports.at(id);
	else
		return {};
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

std::optional<Viewport> Window::getMainViewport() const
{
	if (m_viewports.find(ViewportType::Main) != m_viewports.end())
		return m_viewports.at(ViewportType::Main);
	else
		return {};
}
