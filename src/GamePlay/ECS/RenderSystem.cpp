#include "RenderSystem.hpp"
#include "Components.hpp"
#include "../../Renderer.hpp"
#include "../Render/RenderPipeline.hpp"

namespace ecs {

void RenderSystem::initPipeline()
{
    m_render_pipeline = new RenderPipeline();
    m_render_pipeline->init();
}

void RenderSystem::onUpdate()
{
    m_render_pipeline->render();
}

}