#ifndef RayTracingRenderPath_hpp
#define RayTracingRenderPath_hpp

#include "RenderPath.hpp"

class RayTracingRenderPath : public RenderPath {
public:
    RayTracingRenderPath();
    void init() override;
    void prepareRhi(const std::shared_ptr<Rhi>& rhi) override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;

protected:
    std::unique_ptr<RenderPass> m_ray_tracing_pass;
    std::unique_ptr<RenderPass> m_combine_pass;
};

#endif // !RayTracingRenderPath_hpp