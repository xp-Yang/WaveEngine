#ifndef Window_hpp
#define Window_hpp

struct GLFWwindow;
class FileDialog;
class Window {
public:
    Window(int width, int height);
    void shutdown();
    bool shouldClose() const;
    void swapBuffer() const;
    void* getNativeWindowHandle() const;
    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;

    FileDialog* createFileDialog();

private:
    GLFWwindow* m_window{ nullptr };
    int m_width;
    int m_height;
};

#endif
