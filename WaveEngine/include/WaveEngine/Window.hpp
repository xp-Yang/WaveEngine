#ifndef Window_hpp
#define Window_hpp

#include <string>
#include <optional>
#include <unordered_map>

extern float WINDOW_WIDTH;
extern float WINDOW_HEIGHT;
extern float ASPECT_RATIO;

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

    float AspectRatio() {
        return (float)width / (float)height;
    }
};

enum ViewportType {
    Main,
    Pick,
    Shadow,
    RayTracing,
    Debug,
};

struct GLFWwindow;
// TODO ���ǶԷ�GLFWwindow����չ
class Window {
public:
    Window(int width, int height);
    void shutdown();
    bool shouldClose() const;
    void swapBuffer() const;
    void setMainViewport(const Viewport& viewport);
    void setViewport(ViewportType id, const Viewport& viewport);
    std::optional<Viewport> getViewport(ViewportType id) const;
    std::optional<Viewport> getMainViewport() const;
    GLFWwindow* getNativeWindowHandle() const;
    int getWidth() const;
    int getHeight() const;

private:
    GLFWwindow* m_window{ nullptr };
    int m_width;
    int m_height;
    std::unordered_map<ViewportType, Viewport> m_viewports; // always has one main viewport
};

#endif
