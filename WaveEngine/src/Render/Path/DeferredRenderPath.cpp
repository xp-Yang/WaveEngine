#include "DeferredRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/WireFramePass.hpp"
#include "../Pass/CheckerBoardPass.hpp"
#include "../Pass/NormalPass.hpp"
#include "../Pass/GBufferPass.hpp"
#include "../Pass/LightingPass.hpp"
#include "../Pass/BrightPass.hpp"
#include "../Pass/BlurPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/EdgeDetectionPass.hpp"
#include "../Pass/ScreenPass.hpp"

DeferredRenderPath::DeferredRenderPath()
{
    m_picking_pass = std::make_unique<PickingPass>();
    m_wireframe_pass = std::make_unique<WireFramePass>();
    m_checkerboard_pass = std::make_unique<CheckerBoardPass>();
    m_normal_pass = std::make_unique<NormalPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_gbuffer_pass = std::make_unique<GBufferPass>();
    m_lighting_pass = std::make_unique<LightingPass>();
    m_bright_pass = std::make_unique<BrightPass>();
    m_blur_pass = std::make_unique<BlurPass>();
    m_edge_detection_pass = std::make_unique<EdgeDetectionPass>();
    m_screen_pass = std::make_unique<ScreenPass>();
}

void DeferredRenderPath::init()
{
    m_picking_pass->init();
    m_wireframe_pass->init();
    m_checkerboard_pass->init();
    m_normal_pass->init();
    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_lighting_pass->init();
    m_bright_pass->init();
    m_blur_pass->init();
    m_edge_detection_pass->init();
    m_screen_pass->init();
}

void DeferredRenderPath::prepareRhi(const std::shared_ptr<Rhi>& rhi)
{
    m_picking_pass->prepareRhi(rhi);
    m_wireframe_pass->prepareRhi(rhi);
    m_checkerboard_pass->prepareRhi(rhi);
    m_normal_pass->prepareRhi(rhi);
    m_shadow_pass->prepareRhi(rhi);
    m_gbuffer_pass->prepareRhi(rhi);
    m_lighting_pass->prepareRhi(rhi);
    m_bright_pass->prepareRhi(rhi);
    m_blur_pass->prepareRhi(rhi);
    m_edge_detection_pass->prepareRhi(rhi);
    m_screen_pass->prepareRhi(rhi);
}

void DeferredRenderPath::prepareScreenQuadData(const std::shared_ptr<RenderMeshData>& screen_quad_data)
{
    m_picking_pass->prepareScreenQuadData(screen_quad_data);
    m_wireframe_pass->prepareScreenQuadData(screen_quad_data);
    m_checkerboard_pass->prepareScreenQuadData(screen_quad_data);
    m_normal_pass->prepareScreenQuadData(screen_quad_data);
    m_shadow_pass->prepareScreenQuadData(screen_quad_data);
    m_gbuffer_pass->prepareScreenQuadData(screen_quad_data);
    m_lighting_pass->prepareScreenQuadData(screen_quad_data);
    m_bright_pass->prepareScreenQuadData(screen_quad_data);
    m_blur_pass->prepareScreenQuadData(screen_quad_data);
    m_edge_detection_pass->prepareScreenQuadData(screen_quad_data);
    m_screen_pass->prepareScreenQuadData(screen_quad_data);
}

void DeferredRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_wireframe_pass->prepareRenderSourceData(render_source_data);
    m_checkerboard_pass->prepareRenderSourceData(render_source_data);
    m_normal_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_gbuffer_pass->prepareRenderSourceData(render_source_data);
    m_lighting_pass->prepareRenderSourceData(render_source_data);
    m_bright_pass->prepareRenderSourceData(render_source_data);
    m_blur_pass->prepareRenderSourceData(render_source_data);
    m_edge_detection_pass->prepareRenderSourceData(render_source_data);
    m_screen_pass->prepareRenderSourceData(render_source_data);
}

void DeferredRenderPath::render()
{
    auto screen_pass = static_cast<ScreenPass*>(m_screen_pass.get());

    m_picking_pass->draw();

    if (m_render_params.normal_debug)
        m_normal_pass->draw();
    else
        m_normal_pass->clear();
    if (m_render_params.wireframe) {
        m_wireframe_pass->draw();
        screen_pass->setInputPasses({ m_wireframe_pass.get(), m_normal_pass.get() });
        screen_pass->draw();
        return;
    }
    if (m_render_params.checkerboard) {
        m_checkerboard_pass->draw();
        screen_pass->setInputPasses({ m_checkerboard_pass.get(), m_normal_pass.get() });
        screen_pass->draw();
        return;
    }

    if (m_render_params.shadow) {
        //static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(m_render_params.shadow_map_sample_count);
        m_shadow_pass->draw();
    }
    else
        m_shadow_pass->clear();

    m_gbuffer_pass->draw();

    auto lighting_pass = static_cast<LightingPass*>(m_lighting_pass.get());
    lighting_pass->enableSkybox(m_render_params.skybox);
    lighting_pass->enablePBR(m_render_params.pbr);
    lighting_pass->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    lighting_pass->setInputPasses({ m_gbuffer_pass.get(), m_shadow_pass.get()});
    lighting_pass->draw();

    if (m_render_params.bloom) {
        m_bright_pass->setInputPasses({ m_lighting_pass.get() });
        m_bright_pass->draw();
        m_blur_pass->setInputPasses({ m_bright_pass.get() });
        m_blur_pass->draw();
    }
    else {
        m_bright_pass->clear();
        m_blur_pass->clear();
    }

    m_edge_detection_pass->setInputPasses({ m_lighting_pass.get() }); // need the depth from the lighting pass
    m_edge_detection_pass->draw();

    screen_pass->setInputPasses({ m_lighting_pass.get(), m_normal_pass.get(), m_blur_pass.get(), m_edge_detection_pass.get() });
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

unsigned int DeferredRenderPath::getBrightTexture()
{
    return m_bright_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}

unsigned int DeferredRenderPath::getBlurredTexture()
{
    return m_blur_pass->getFrameBuffer()->colorAttachmentAt(0)->texture()->id();
}
