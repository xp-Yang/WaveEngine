#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "RenderPipeline.hpp"

class RenderSystem {
public:
    void initPipeline();
    void setRenderParams(const RenderParams& params);
    void onUpdate();

private:
    std::unique_ptr<RenderPipeline> m_render_pipeline;
};

#endif // !RenderSystem_hpp
