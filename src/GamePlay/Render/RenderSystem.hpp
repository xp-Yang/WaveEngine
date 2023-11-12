#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "RenderPipeline.hpp"

class RenderSystem {
public:
    RenderSystem();
    void setRenderParams(const RenderParams& params);
    void onUpdate();

private:
    std::unique_ptr<RenderPipeline> m_render_pipeline;
};

#endif // !RenderSystem_hpp
