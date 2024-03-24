#include "RenderSystem.hpp"
#include "Path/ForwardRenderPath.hpp"
#include "Path/DeferredRenderPath.hpp"
#include "Path/RayTracingRenderPath.hpp"

RenderSystem::RenderSystem()
{
    m_forward_path = std::make_shared<ForwardRenderPath>();
    m_forward_path->init();

    m_deferred_path = std::make_shared<DeferredRenderPath>();
    m_deferred_path->init();

    m_ray_tracing_path = std::make_shared<RayTracingRenderPath>();
    m_ray_tracing_path->init();

    setRenderPathType(RenderPath::Type::Deferred);
}

RenderPath::Type RenderSystem::getRenderPathType()
{
    return m_path_type;
}

void RenderSystem::setRenderPathType(RenderPath::Type type)
{
    m_path_type = type;

    switch (m_path_type)
    {
    case RenderPath::Type::Forward:
        m_curr_path = m_forward_path;
        break;
    case RenderPath::Type::Deferred:
        m_curr_path = m_deferred_path;
        break;
    case RenderPath::Type::RayTracing:
        m_curr_path = m_ray_tracing_path;
        break;
    default:
        break;
    }
}

void RenderSystem::setRenderParams(const RenderParams& params)
{
    m_curr_path->setRenderParams(params);
}

void RenderSystem::onUpdate()
{
    m_curr_path->render();
}
