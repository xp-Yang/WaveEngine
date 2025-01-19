#ifndef ForwardRenderPath_hpp
#define ForwardRenderPath_hpp

#include "RenderPath.hpp"

class RenderSystem;
class ForwardRenderPath : public RenderPath {
public:
    ForwardRenderPath(RenderSystem* render_system);
    void render() override;

protected:
    RenderSystem* ref_render_system{ nullptr };
};

#endif // !ForwardRenderPath_hpp