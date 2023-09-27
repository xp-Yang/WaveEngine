#include "RenderPipeline.hpp"
#include "ShadowPass.hpp"
#include "GBufferPass.hpp"
#include "MainCameraPass.hpp"
#include "PickingPass.hpp"
#include "ScreenPass.hpp"
#include "../../../imgui/imgui.h"

void RenderPipeline::init()
{
    m_main_camera_pass = std::make_shared<MainCameraPass>();
    m_shadow_pass = std::make_shared<ShadowPass>();
    m_gbuffer_pass = std::make_shared<GBufferPass>();
    m_picking_pass = std::make_shared<PickingPass>();
    m_screen_pass = std::make_shared<ScreenPass>();
    m_main_camera_pass->init();
    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_picking_pass->init();
    m_screen_pass->init();
}

void RenderPipeline::setRenderParams(const RenderParams& params)
{
    m_render_params = params;
}

void RenderPipeline::render()
{
    if (m_render_params.pipeline_type == PIPELINE_TYPE::FORWARD) {
        if (!ImGui::GetIO().WantCaptureMouse) {
            m_picking_pass->draw();
        }
        if (m_render_params.shadow) {
            m_shadow_pass->configSamples(m_render_params.shadow_map_sample_count);
            m_shadow_pass->draw();
            m_main_camera_pass->prepare(m_shadow_pass->getFrameBuffer());
        }
        else {
            m_main_camera_pass->prepare(nullptr);
        }
        static_cast<MainCameraPass*>(m_main_camera_pass.get())->configShader(m_render_params.reflection, m_render_params.normal_debug, m_render_params.wireframe);
        m_main_camera_pass->configSamples(m_render_params.msaa_sample_count);
        m_main_camera_pass->draw();

        m_screen_pass->prepare(m_main_camera_pass->getFrameBuffer());
        m_screen_pass->draw();
    }
    else if (m_render_params.pipeline_type == PIPELINE_TYPE::DEFERRED) {
    
    }
}
