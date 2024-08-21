#ifndef ImGuiImGuiViewPortWindow_hpp
#define ImGuiImGuiViewPortWindow_hpp

#if ENABLE_ECS
#include "Logical/FrameWork/ECS/World.hpp"
#include "Logical/FrameWork/ECS/Components.hpp"
#endif
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

enum ViewportType : unsigned int {
    Main,
    Pick,
    Shadow,
    RayTracing,
};

class ImGuiViewPortWindow;
class ImGuiViewPortWindowManager {
public:
	ImGuiViewPortWindowManager();
	~ImGuiViewPortWindowManager();
	void render();

    Viewport getViewport(ViewportType type) const;
    Viewport getMainViewport() const;

private:
	std::vector<ImGuiViewPortWindow*> m_windows;
};

#endif