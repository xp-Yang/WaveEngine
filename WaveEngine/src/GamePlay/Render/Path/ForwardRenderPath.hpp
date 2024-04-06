#ifndef ForwardRenderPath_hpp
#define ForwardRenderPath_hpp

#include "RenderPath.hpp"
#include "../Pass/RenderPass.hpp"

class ForwardRenderPath : public RenderPath {
public:
    void init() override;
    void render() override;

protected:
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_main_camera_pass;
    std::unique_ptr<RenderPass> m_screen_pass;

};

#endif // !ForwardRenderPath_hpp