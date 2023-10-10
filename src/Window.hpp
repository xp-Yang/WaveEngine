#ifndef Window_hpp
#define Window_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1600.0f
#define WINDOW_HEIGHT 900.0f

struct Viewport {
    int x;
    int y;
    int width;
    int height;
};

// TODO 考虑对非GLFWwindow的扩展
class Window {
public:
    void create(int width, int height);
    void shutdown();
    bool shouldClose() const;
    void pollEvents() const;
    void setMainViewport(const Viewport& viewport);
    GLFWwindow* getNativeWindowHandle() const;
    int getWidth() const;
    int getHeight() const;
    Viewport getMainViewport() const;
private:
    GLFWwindow* m_window{ nullptr };
    int m_width;
    int m_height;
    Viewport m_main_viewport;
};

#endif
