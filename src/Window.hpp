#ifndef Window_hpp
#define Window_hpp

struct Viewport {
    int x;
    int y;
    int width;
    int height;
};

struct GLFWwindow;
class Window {
public:
    void create(int width, int height);
    void shutdown();
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
