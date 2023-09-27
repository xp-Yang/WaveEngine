#include "RenderSystem.hpp"
#include "Components.hpp"

namespace ecs {

void RenderSystem::initPipeline()
{
    m_render_pipeline = new RenderPipeline();
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

}