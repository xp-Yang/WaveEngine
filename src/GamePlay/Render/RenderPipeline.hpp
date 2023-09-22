#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include <memory>

struct RenderParams {
    int     msaa_sample_count = 1;
    bool    shadow = true;
    int     shadow_map_sample_count = 1;
    bool    reflection = false;
    bool    normal_debug = false;
    bool    wireframe = false;
    int     pixelate_level = 1;
};

class RenderPass;
class RenderPipeline {
public:
    void init();
    void setRenderParams(const RenderParams& params);
    void render(); // deferred_render()s/forward_render();
private:
    std::shared_ptr<RenderPass> m_shadow_pass;
    std::shared_ptr<RenderPass> m_main_camera_pass;
    std::shared_ptr<RenderPass> m_picking_pass;
    std::shared_ptr<RenderPass> m_post_processing_pass;
    std::shared_ptr<RenderPass> m_screen_pass;
    RenderParams m_render_params;
};

#endif // !RenderPipeline_hpp
