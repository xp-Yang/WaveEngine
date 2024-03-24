#include "DeferredRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/GBufferPass.hpp"
#include "../Pass/LightingPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/ScreenPass.hpp"

void DeferredRenderPath::init()
{
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_gbuffer_pass = std::make_unique<GBufferPass>();
    m_lighting_pass = std::make_unique<LightingPass>();
    m_picking_pass = std::make_unique<PickingPass>();
    m_screen_pass = std::make_unique<ScreenPass>();

    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_lighting_pass->init();
    m_picking_pass->init();
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

    static_cast<LightingPass*>(m_lighting_pass.get())->enableSkybox(m_render_params.skybox);
    static_cast<LightingPass*>(m_lighting_pass.get())->enableNormal(m_render_params.normal_debug);
    static_cast<LightingPass*>(m_lighting_pass.get())->enableWireframe(m_render_params.wireframe);
    static_cast<LightingPass*>(m_lighting_pass.get())->enableCheckerboard(m_render_params.checkerboard);
    static_cast<LightingPass*>(m_lighting_pass.get())->enablePBR(m_render_params.pbr);
    static_cast<LightingPass*>(m_lighting_pass.get())->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    if (m_render_params.shadow) {
        static_cast<LightingPass*>(m_lighting_pass.get())->prepare(m_gbuffer_pass->getFrameBuffer(), m_shadow_pass->getFrameBuffer());
    }
    else {
        m_lighting_pass->prepare(m_gbuffer_pass->getFrameBuffer());
    }
    m_lighting_pass->draw();

    m_screen_pass->prepare(m_lighting_pass->getFrameBuffer());
    m_screen_pass->draw();
}
