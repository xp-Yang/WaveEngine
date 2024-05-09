#ifndef RayTracingRenderPath_hpp
#define RayTracingRenderPath_hpp

#include "RenderPath.hpp"

class RayTracingRenderPath : public RenderPath {
public:
    void init() override;
    void prepareScreenQuadData(const std::shared_ptr<RenderSubMeshData>& screen_quad_data) override;
    void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) override;
    void render() override;

protected:
    std::unique_ptr<RenderPass> m_ray_tracing_pass;
    std::unique_ptr<RenderPass> m_screen_pass;
};

#endif // !RayTracingRenderPath_hpp