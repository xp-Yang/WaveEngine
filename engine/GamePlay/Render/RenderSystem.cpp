#include "RenderSystem.hpp"

RenderSystem::RenderSystem()
{
    m_render_pipeline = std::make_unique<RenderPipeline>();
    m_render_pipeline->init();
}

void RenderSystem::setRenderParams(const RenderParams& params)
{
    m_render_pipeline->setRenderParams(params);
}

void RenderSystem::onUpdate()
{
    m_render_pipeline->render();
}
