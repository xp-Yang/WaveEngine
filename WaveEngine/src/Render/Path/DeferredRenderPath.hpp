#ifndef DeferredRenderPath_hpp
#define DeferredRenderPath_hpp

#include "RenderPath.hpp"
#include "Render/Pass/RenderPass.hpp"

class DeferredRenderPath : public RenderPath {
public:
    DeferredRenderPath();
    void init() override;
    void prepareRhi(const std::shared_ptr<Rhi>& rhi) override;
    void prepareScreenQuadData(const std::shared_ptr<RenderMeshData>& screen_quad_data) override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;
    unsigned int getSceneTexture() override;
    unsigned int getPickingTexture() override;
    unsigned int getPickingFBO() override;
    unsigned int getShadowTexture() override;

protected:
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_gbuffer_pass;
    std::unique_ptr<RenderPass> m_lighting_pass;
    std::unique_ptr<RenderPass> m_blur_pass;
    std::unique_ptr<RenderPass> m_edge_detection_pass;
    std::unique_ptr<RenderPass> m_screen_pass;

};

#endif // !DeferredRenderPath_hpp