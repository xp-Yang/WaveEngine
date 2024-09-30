#ifndef DeferredRenderPath_hpp
#define DeferredRenderPath_hpp

#include "RenderPath.hpp"

class DeferredRenderPath : public RenderPath {
public:
    DeferredRenderPath();
    void init() override;
    void prepareRhi(const std::shared_ptr<Rhi>& rhi) override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;
    unsigned int getPickingFBO() override;
    RhiTexture* renderPassTexture(RenderPass::Type render_pass_type) override;

protected:
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_wireframe_pass;
    std::unique_ptr<RenderPass> m_checkerboard_pass;
    std::unique_ptr<RenderPass> m_normal_pass;
    std::unique_ptr<RenderPass> m_gbuffer_pass;
    std::unique_ptr<RenderPass> m_lighting_pass;
    std::unique_ptr<RenderPass> m_bloom_pass;
    std::unique_ptr<RenderPass> m_outline_pass;
    std::unique_ptr<RenderPass> m_combine_pass;

};

#endif // !DeferredRenderPath_hpp