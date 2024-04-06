#include "DeferredRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/GBufferPass.hpp"
#include "../Pass/LightingPass.hpp"
#include "../Pass/BlurPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/EdgeDetectionPass.hpp"
#include "../Pass/ScreenPass.hpp"

void DeferredRenderPath::init()
{
    m_picking_pass = std::make_unique<PickingPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_gbuffer_pass = std::make_unique<GBufferPass>();
    m_lighting_pass = std::make_unique<LightingPass>();
    m_blur_pass = std::make_unique<BlurPass>();
    m_edge_detection_pass = std::make_unique<EdgeDetectionPass>();
    m_screen_pass = std::make_unique<ScreenPass>();

    m_picking_pass->init();
    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_lighting_pass->init();
    m_blur_pass->init();
    m_edge_detection_pass->init();
    m_screen_pass->init();


    static_cast<ScreenPass*>(m_screen_pass.get())->setPickView(m_picking_pass->getFrameBuffer());
    static_cast<ScreenPass*>(m_screen_pass.get())->setShadowView(m_shadow_pass->getFrameBuffer());
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
    if (m_render_params.shadow) {
        lighting_pass->prepare(m_gbuffer_pass->getFrameBuffer(), m_shadow_pass->getFrameBuffer());
    }
    else {
        m_lighting_pass->prepare(m_gbuffer_pass->getFrameBuffer());
    }
    m_lighting_pass->draw();

    static_cast<BlurPass*>(m_blur_pass.get())->setBrightMap(lighting_pass->getBrightMap());
    m_blur_pass->draw();

    m_edge_detection_pass->prepare(nullptr);
    m_edge_detection_pass->draw();

    auto screen_pass = static_cast<ScreenPass*>(m_screen_pass.get());
    screen_pass->setBlurredBrightMap(static_cast<BlurPass*>(m_blur_pass.get())->getBlurredBrightMap());
    screen_pass->setBorderMap(static_cast<EdgeDetectionPass*>(m_edge_detection_pass.get())->getFrameBuffer()->getFirstAttachmentOf(AttachmentType::RGB16F).getMap());
    screen_pass->prepare(m_lighting_pass->getFrameBuffer());
    screen_pass->draw();
}
