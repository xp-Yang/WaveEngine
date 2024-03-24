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

void RayTracingRenderPath::render()
{
    m_ray_tracing_pass->draw();

    //m_screen_pass->prepare(m_ray_tracing_pass->getFrameBuffer());
    //m_screen_pass->draw();
}
