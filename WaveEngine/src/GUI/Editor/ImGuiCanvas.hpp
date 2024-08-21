#ifndef ImGuiCanvas_hpp
#define ImGuiCanvas_hpp

#include "Core/Common.hpp"

struct Viewport {
    Viewport() = default;
    Viewport(int _x, int _y, int _width, int _height)
        : x(_x)
        , y(_y)
        , width(_width)
        , height(_height)
    {}
    Viewport(const Viewport& _rhs)
        : x(_rhs.x)
        , y(_rhs.y)
        , width(_rhs.width)
        , height(_rhs.height)
    {}
    int x;
    int y;
    int width;
    int height;

    float AspectRatio() {
        return (float)width / (float)height;
    }
};

enum CanvasType : unsigned int {
    Main,
    Pick,
    Shadow,
    RayTracing,
};

class ImGuiCanvas;
class ImGuiCanvasManager {
public:
	ImGuiCanvasManager();
	~ImGuiCanvasManager();
	void render();

    Viewport getViewport(CanvasType type) const;
    Viewport getMainViewport() const;

private:
	std::vector<ImGuiCanvas*> m_canvases;
};

#endif