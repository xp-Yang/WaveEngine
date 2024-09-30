#include "RayTracingRenderPath.hpp"

#include "../Pass/RayTracingPass.hpp"
#include "../Pass/CombinePass.hpp"

RayTracingRenderPath::RayTracingRenderPath()
{
    m_ray_tracing_pass = std::make_unique<RayTracingPass>();
    m_combine_pass = std::make_unique<CombinePass>();
}

void RayTracingRenderPath::init()
{
    m_ray_tracing_pass->init();
    m_combine_pass->init();
}

void RayTracingRenderPath::prepareRhi(const std::shared_ptr<Rhi>& rhi)
{
    m_ray_tracing_pass->prepareRhi(rhi);
    m_combine_pass->prepareRhi(rhi);
}

void RayTracingRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_ray_tracing_pass->prepareRenderSourceData(render_source_data);
    m_combine_pass->prepareRenderSourceData(render_source_data);
}

void RayTracingRenderPath::render()
{
    m_ray_tracing_pass->draw();

    m_combine_pass->setInputPasses({ m_ray_tracing_pass.get() });
    m_combine_pass->draw();
}
