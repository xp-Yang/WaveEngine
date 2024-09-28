#include "DeferredRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/GBufferPass.hpp"
#include "../Pass/LightingPass.hpp"
#include "../Pass/BlurPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/EdgeDetectionPass.hpp"
#include "../Pass/ScreenPass.hpp"

DeferredRenderPath::DeferredRenderPath()
{
    m_picking_pass = std::make_unique<PickingPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_gbuffer_pass = std::make_unique<GBufferPass>();
    m_lighting_pass = std::make_unique<LightingPass>();
    m_blur_pass = std::make_unique<BlurPass>();
    m_edge_detection_pass = std::make_unique<EdgeDetectionPass>();
    m_screen_pass = std::make_unique<ScreenPass>();
}

void DeferredRenderPath::init()
{
    m_picking_pass->init();
    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_lighting_pass->init();
    m_blur_pass->init();
    m_edge_detection_pass->init();
    m_screen_pass->init();
}

void DeferredRenderPath::prepareRhi(const std::shared_ptr<Rhi>& rhi)
{
    m_picking_pass->prepareRhi(rhi);
    m_shadow_pass->prepareRhi(rhi);
    m_gbuffer_pass->prepareRhi(rhi);
    m_lighting_pass->prepareRhi(rhi);
    m_blur_pass->prepareRhi(rhi);
    m_edge_detection_pass->prepareRhi(rhi);
    m_screen_pass->prepareRhi(rhi);
}

void DeferredRenderPath::prepareScreenQuadData(const std::shared_ptr<RenderMeshData>& screen_quad_data)
{
    m_picking_pass->prepareScreenQuadData(screen_quad_data);
    m_shadow_pass->prepareScreenQuadData(screen_quad_data);
    m_gbuffer_pass->prepareScreenQuadData(screen_quad_data);
    m_lighting_pass->prepareScreenQuadData(screen_quad_data);
    m_blur_pass->prepareScreenQuadData(screen_quad_data);
    m_edge_detection_pass->prepareScreenQuadData(screen_quad_data);
    m_screen_pass->prepareScreenQuadData(screen_quad_data);
}

void DeferredRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_gbuffer_pass->prepareRenderSourceData(render_source_data);
    m_lighting_pass->prepareRenderSourceData(render_source_data);
    m_blur_pass->prepareRenderSourceData(render_source_data);
    m_edge_detection_pass->prepareRenderSourceData(render_source_data);
    m_screen_pass->prepareRenderSourceData(render_source_data);
}

void DeferredRenderPath::render()
{
    m_picking_pass->draw();

    if (m_render_params.shadow) {
        static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(m_render_params.shadow_map_sample_count);
        m_shadow_pass->draw();
    }
    else {
        static_cast<ShadowPass*>(m_shadow_pass.get())->clear();
    }

    m_gbuffer_pass->draw();

    auto lighting_pass = static_cast<LightingPass*>(m_lighting_pass.get());
    lighting_pass->enableSkybox(m_render_params.skybox);
    lighting_pass->enableNormal(m_render_params.normal_debug);
    lighting_pass->enableWireframe(m_render_params.wireframe);
    lighting_pass->enableCheckerboard(m_render_params.checkerboard);
    lighting_pass->enablePBR(m_render_params.pbr);
    lighting_pass->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    lighting_pass->setInputPasses({ m_gbuffer_pass.get(), m_shadow_pass.get()});
    lighting_pass->draw();

    m_blur_pass->setInputPasses({ m_lighting_pass.get() });
    m_blur_pass->draw();

    m_edge_detection_pass->setInputPasses({ m_lighting_pass.get() });
    m_edge_detection_pass->draw();

    auto screen_pass = static_cast<ScreenPass*>(m_screen_pass.get());
    screen_pass->setInputPasses({ m_lighting_pass.get(), m_blur_pass.get(), m_edge_detection_pass.get() });
    screen_pass->draw();
}

unsigned int DeferredRenderPath::getSceneTexture()
{
    return m_screen_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}

unsigned int DeferredRenderPath::getPickingTexture()
{
    return m_picking_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}

unsigned int DeferredRenderPath::getPickingFBO()
{
    return m_picking_pass->getFrameBuffer()->id();
}

unsigned int DeferredRenderPath::getShadowTexture()
{
    return m_shadow_pass->getFrameBuffer()->depthAttachment()->texture()->id();
}

unsigned int DeferredRenderPath::getGBufferTexture()
{
    return m_gbuffer_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}

unsigned int DeferredRenderPath::getLightingTexture()
{
    return m_lighting_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}

unsigned int DeferredRenderPath::getBlurredTexture()
{
    return m_blur_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}
