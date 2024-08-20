#ifndef ForwardRenderPath_hpp
#define ForwardRenderPath_hpp

#include "RenderPath.hpp"
#include "Render/Pass/RenderPass.hpp"

class ForwardRenderPath : public RenderPath {
public:
    ForwardRenderPath();
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
    std::unique_ptr<RenderPass> m_main_camera_pass;
    std::unique_ptr<RenderPass> m_screen_pass;

};

#endif // !ForwardRenderPath_hpp