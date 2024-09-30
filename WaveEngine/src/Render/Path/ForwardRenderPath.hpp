#ifndef ForwardRenderPath_hpp
#define ForwardRenderPath_hpp

#include "RenderPath.hpp"

class RenderSystem;
class ForwardRenderPath : public RenderPath {
public:
    ForwardRenderPath(RenderSystem* render_system);
    void init() override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;
    unsigned int getPickingFBO() override;
    RhiTexture* renderPassTexture(RenderPass::Type render_pass_type) override;

protected:
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_main_camera_pass;
    std::unique_ptr<RenderPass> m_combine_pass;

    RenderSystem* ref_render_system{ nullptr };
};

#endif // !ForwardRenderPath_hpp