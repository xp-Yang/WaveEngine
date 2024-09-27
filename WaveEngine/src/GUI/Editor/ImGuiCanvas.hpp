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
    GBuffer,
    Lighting,
    RayTracing,
};

class ImGuiCanvas {
public:
    virtual void render() = 0;
    void setViewPort(const Viewport& viewport) { m_viewport = viewport; }
    Viewport getViewport() const { return m_viewport; }
    CanvasType type() const { return m_type; }

protected:
    CanvasType m_type;
    Viewport m_viewport;
};

class MainCanvas : public ImGuiCanvas {
public:
    enum class ToolbarType : int {
        Translate,
        Rotate,
        Scale,
    };
    MainCanvas() { m_type = CanvasType::Main; }
    void render() override;

protected:
    void renderGizmos();

private:
    ToolbarType m_toolbar_type{ ToolbarType::Translate };
};

class PickingCanvas : public ImGuiCanvas {
public:
    PickingCanvas() { m_type = CanvasType::Pick; }
    void render() override;
};

class ShadowCanvas : public ImGuiCanvas {
public:
    ShadowCanvas() { m_type = CanvasType::Shadow; }
    void render() override;
};

class GBufferCanvas : public ImGuiCanvas {
public:
    GBufferCanvas() { m_type = CanvasType::GBuffer; }
    void render() override;
};

class LightingCanvas : public ImGuiCanvas {
public:
    LightingCanvas() { m_type = CanvasType::Lighting; }
    void render() override;
};

#endif