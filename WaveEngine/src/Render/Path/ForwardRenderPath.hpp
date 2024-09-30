#ifndef ForwardRenderPath_hpp
#define ForwardRenderPath_hpp

#include "RenderPath.hpp"

class ForwardRenderPath : public RenderPath {
public:
    ForwardRenderPath();
    void init() override;
    void prepareRhi(const std::shared_ptr<Rhi>& rhi) override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;
    unsigned int getPickingFBO() override;
    RhiTexture* renderPassTexture(RenderPass::Type render_pass_type) override;

protected:
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_main_camera_pass;
    std::unique_ptr<RenderPass> m_combine_pass;

};

#endif // !ForwardRenderPath_hpp