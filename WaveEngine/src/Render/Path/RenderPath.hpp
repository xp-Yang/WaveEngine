#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include "Core/Common.hpp"
#include "Render/Pass/RenderPass.hpp"

// Interface class
class RenderPath {
public:
    virtual void init() = 0;
    virtual void prepareRhi(const std::shared_ptr<Rhi>& rhi) = 0;
    virtual void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) = 0;
    virtual void render() = 0;
    virtual unsigned int getPickingFBO() { return 0; };
    virtual RhiTexture* renderPassTexture(RenderPass::Type render_pass_type) { return nullptr; }
};

#endif // !RenderPipeline_hpp
