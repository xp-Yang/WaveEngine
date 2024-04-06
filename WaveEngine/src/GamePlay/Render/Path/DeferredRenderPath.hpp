#ifndef DeferredRenderPath_hpp
#define DeferredRenderPath_hpp

#include "RenderPath.hpp"
#include "../Pass/RenderPass.hpp"

class DeferredRenderPath : public RenderPath {
public:
    void init() override;
    void render() override;

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