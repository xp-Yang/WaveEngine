#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "GamePlay/Render/RenderPipeline.hpp"

namespace ecs {

class RenderSystem {
public:
    void initPipeline();
    void setRenderParams(const RenderParams& params);
    void onUpdate();

private:
    RenderPipeline* m_render_pipeline{ nullptr };
};

}

#endif // !RenderSystem_hpp
