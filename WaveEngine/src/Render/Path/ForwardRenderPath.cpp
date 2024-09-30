#include "ForwardRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/MeshForwardLightingPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/CombinePass.hpp"

#include "EngineAPI.hpp"

ForwardRenderPath::ForwardRenderPath() {
    m_picking_pass = std::make_unique<PickingPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_main_camera_pass = std::make_unique<MeshForwardLightingPass>();
    m_combine_pass = std::make_unique<CombinePass>();
}

void ForwardRenderPath::init()
{
    m_picking_pass->init();
    m_shadow_pass->init();
    m_main_camera_pass->init();
    m_combine_pass->init();
}

void ForwardRenderPath::prepareRhi(const std::shared_ptr<Rhi>& rhi)
{
    m_picking_pass->prepareRhi(rhi);
    m_shadow_pass->prepareRhi(rhi);
    m_main_camera_pass->prepareRhi(rhi);
    m_combine_pass->prepareRhi(rhi);
}

void ForwardRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_main_camera_pass->prepareRenderSourceData(render_source_data);
    m_combine_pass->prepareRenderSourceData(render_source_data);
}

void ForwardRenderPath::render()
{
    auto render_params = GetApp().renderSystem()->renderParams();

    m_picking_pass->draw();

    if (render_params.shadow) {
        static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(render_params.shadow_map_sample_count);
        m_shadow_pass->draw();
    }
    else {
        static_cast<ShadowPass*>(m_shadow_pass.get())->clear();
    }

    m_main_camera_pass->setInputPasses({ m_shadow_pass.get() });

    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->configShader(render_params.skybox, render_params.reflection, render_params.normal_debug, render_params.wireframe);
    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->configSamples(render_params.msaa_sample_count);
    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    m_main_camera_pass->draw();

    m_combine_pass->setInputPasses({ m_main_camera_pass.get() });
    m_combine_pass->draw();
}

unsigned int ForwardRenderPath::getPickingFBO()
{
    return m_picking_pass->getFrameBuffer()->id();
}

RhiTexture* ForwardRenderPath::renderPassTexture(RenderPass::Type render_pass_type)
{
    return nullptr;
}