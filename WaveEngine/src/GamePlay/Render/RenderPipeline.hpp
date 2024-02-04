#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include "RenderPass.hpp"
#include <memory>

enum class PIPELINE_TYPE{
    FORWARD,
    DEFERRED,
};
struct RenderParams {
    PIPELINE_TYPE pipeline_type = PIPELINE_TYPE::DEFERRED;
    int     msaa_sample_count = 4;
    bool    shadow = true;
    int     shadow_map_sample_count = 1;
    // for MainCameraPass
    bool    reflection = false;
    bool    normal_debug = false;
    bool    wireframe = false;
    int     pixelate_level = 1;
};

class RenderPipeline {
public:
    void init();
    void setRenderParams(const RenderParams& params);
    void render(); // deferred_render()s/forward_render();

private:
    std::unique_ptr<RenderPass> m_shadow_pass;
    std::unique_ptr<RenderPass> m_gbuffer_pass;
    std::unique_ptr<RenderPass> m_lighting_pass;
    std::unique_ptr<RenderPass> m_main_camera_pass;
    std::unique_ptr<RenderPass> m_picking_pass;
    std::unique_ptr<RenderPass> m_post_processing_pass;
    std::unique_ptr<RenderPass> m_screen_pass;

    std::unique_ptr<RenderPass> m_ray_tracing_pass;
    RenderParams m_render_params;
};

#endif // !RenderPipeline_hpp
