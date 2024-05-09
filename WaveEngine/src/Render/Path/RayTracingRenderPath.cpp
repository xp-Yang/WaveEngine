#include "RayTracingRenderPath.hpp"

#include "../Pass/RayTracingPass.hpp"
#include "../Pass/ScreenPass.hpp"

void RayTracingRenderPath::init()
{
    m_ray_tracing_pass = std::make_unique<RayTracingPass>();
    m_ray_tracing_pass->init();

    m_screen_pass = std::make_unique<ScreenPass>();
    m_screen_pass->init();
}

void RayTracingRenderPath::prepareScreenQuadData(const std::shared_ptr<RenderSubMeshData>& screen_quad_data)
{
    m_ray_tracing_pass->prepareScreenQuadData(screen_quad_data);
    m_screen_pass->prepareScreenQuadData(screen_quad_data);
}

void RayTracingRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_ray_tracing_pass->prepareRenderSourceData(render_source_data);
    m_screen_pass->prepareRenderSourceData(render_source_data);
}

void RayTracingRenderPath::render()
{
    m_ray_tracing_pass->draw();

    m_screen_pass->prepare(m_ray_tracing_pass->getFrameBuffer());
    m_screen_pass->draw();
}
