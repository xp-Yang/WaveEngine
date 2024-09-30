#ifndef ImGuiCanvas_hpp
#define ImGuiCanvas_hpp

#include "Core/Common.hpp"
#include "Render/RHI/Viewport.hpp"

enum CanvasType : unsigned int {
    Main,
    Pick,
    Shadow,
    GBuffer,
    Lighting,
    Bright,
    Blurred,
    RayTracing,
};

class ImGuiEditor;
class ImGuiCanvas {
public:
    ImGuiCanvas(ImGuiEditor* parent) : m_parent(parent) {}
    virtual void render() = 0;
    void setViewPort(const Viewport& viewport) { m_viewport = viewport; }
    Viewport getViewport() const { return m_viewport; }
    CanvasType type() const { return m_type; }

protected:
    CanvasType m_type;
    Viewport m_viewport;

    ImGuiEditor* m_parent;
};

class ImGuiToolbar;
class MainCanvas : public ImGuiCanvas {
public:
    MainCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Main; }
    void render() override;

protected:
    ImGuiToolbar* m_toolbar{ nullptr };
};

class PickingCanvas : public ImGuiCanvas {
public:
    PickingCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Pick; }
    void render() override;
};

class ShadowCanvas : public ImGuiCanvas {
public:
    ShadowCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Shadow; }
    void render() override;
};

class GBufferCanvas : public ImGuiCanvas {
public:
    GBufferCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::GBuffer; }
    void render() override;
};

class LightingCanvas : public ImGuiCanvas {
public:
    LightingCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Lighting; }
    void render() override;
};

class BlurredCanvas : public ImGuiCanvas {
public:
    BlurredCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Bright; }
    void render() override;
};

class BrightCanvas : public ImGuiCanvas {
public:
    BrightCanvas(ImGuiEditor* parent) : ImGuiCanvas(parent) { m_type = CanvasType::Blurred; }
    void render() override;
};

#endif