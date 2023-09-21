#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include <memory>

class RenderPass;

class RenderPipeline {
public:
    void init();
    void render(); // deferred_render()s/forward_render();
private:
    std::shared_ptr<RenderPass> m_shadow_pass;
    std::shared_ptr<RenderPass> m_main_camera_pass;
    std::shared_ptr<RenderPass> m_picking_pass;
    //std::shared_ptr<RenderPass> m_post_processing_pass;
    std::shared_ptr<RenderPass> m_screen_pass;
    std::shared_ptr<RenderPass> m_ui_pass;
};

#endif // !RenderPipeline_hpp
