#ifndef RayTracingRenderPath_hpp
#define RayTracingRenderPath_hpp

#include "RenderPath.hpp"

class RayTracingRenderPath : public RenderPath {
public:
    void init() override;
    void render() override;

protected:
    std::unique_ptr<RenderPass> m_ray_tracing_pass;
    std::unique_ptr<RenderPass> m_post_processing_pass;
    std::unique_ptr<RenderPass> m_screen_pass;
};

#endif // !RayTracingRenderPath_hpp