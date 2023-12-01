#ifndef Window_hpp
#define Window_hpp

#include <optional>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern float WINDOW_WIDTH;
extern float WINDOW_HEIGHT;

struct Viewport {
    enum Coordinates : unsigned int {
        GLCoordinates,
        ScreenCoordinates,
    };
    Viewport() = default;
    Viewport(int _x, int _y, int _width, int _height, Coordinates _coordinates)
        : x(_x)
        , y(_y)
        , width(_width)
        , height(_height)
        , coordinates(_coordinates)
    {}
    Viewport(const Viewport& _rhs)
        : x(_rhs.x)
        , y(_rhs.y)
        , width(_rhs.width)
        , height(_rhs.height)
        , coordinates(_rhs.coordinates)
    {}
    int x;
    int y;
    int width;
    int height;
    Coordinates coordinates;

    Viewport& transToScreenCoordinates() {
        if (coordinates == ScreenCoordinates)
            return *this;
        this->y = WINDOW_HEIGHT - this->height - this->y;
        coordinates = ScreenCoordinates;
        return *this;
    }

    Viewport& transToGLCoordinates() {
        if (coordinates == GLCoordinates)
            return *this;
        this->y = WINDOW_HEIGHT - this->height - this->y;
        coordinates = GLCoordinates;
        return *this;
    }
};

// TODO 考虑对非GLFWwindow的扩展
class Window {
public:
    Window(int width, int height);
    void shutdown();
    bool shouldClose() const;
    void update() const;
    void setMainViewport(const Viewport& viewport);
    void setViewport(std::string id, const Viewport& viewport);
    std::optional<Viewport> getViewport(std::string id) const;
    std::optional<Viewport> getMainViewport() const;
    GLFWwindow* getNativeWindowHandle() const;
    int getWidth() const;
    int getHeight() const;
private:
    GLFWwindow* m_window{ nullptr };
    int m_width;
    int m_height;
    std::unordered_map<std::string, Viewport> m_viewports; // always has one main viewport
};

#endif
