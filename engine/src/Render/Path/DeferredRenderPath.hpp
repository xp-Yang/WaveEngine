#ifndef DeferredRenderPath_hpp
#define DeferredRenderPath_hpp

#include "RenderPath.hpp"

class RenderSystem;
class DeferredRenderPath : public RenderPath {
public:
    DeferredRenderPath(RenderSystem* render_system);
    void render() override;

protected:
    RenderSystem* ref_render_system{ nullptr };
};

#endif // !DeferredRenderPath_hpp