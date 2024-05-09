#include "ForwardRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/MainCameraPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/ScreenPass.hpp"

void ForwardRenderPath::init()
{
    m_picking_pass = std::make_unique<PickingPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_main_camera_pass = std::make_unique<MainCameraPass>();
    m_screen_pass = std::make_unique<ScreenPass>();

    m_picking_pass->init();
    m_shadow_pass->init();
    m_main_camera_pass->init();
    m_screen_pass->init();

    static_cast<ScreenPass*>(m_screen_pass.get())->setPickView(m_picking_pass->getFrameBuffer());
    static_cast<ScreenPass*>(m_screen_pass.get())->setShadowView(m_shadow_pass->getFrameBuffer());
}

void ForwardRenderPath::prepareScreenQuadData(const std::shared_ptr<RenderSubMeshData>& screen_quad_data)
{
    m_picking_pass->prepareScreenQuadData(screen_quad_data);
    m_shadow_pass->prepareScreenQuadData(screen_quad_data);
    m_main_camera_pass->prepareScreenQuadData(screen_quad_data);
    m_screen_pass->prepareScreenQuadData(screen_quad_data);
}

void ForwardRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_main_camera_pass->prepareRenderSourceData(render_source_data);
    m_screen_pass->prepareRenderSourceData(render_source_data);
}

void ForwardRenderPath::render()
{
    m_picking_pass->draw();

    if (m_render_params.shadow) {
        static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(m_render_params.shadow_map_sample_count);
        m_shadow_pass->draw();
    }
    else {
        static_cast<ShadowPass*>(m_shadow_pass.get())->clear();
    }

    if (m_render_params.shadow) {
        m_main_camera_pass->prepare(m_shadow_pass->getFrameBuffer());
    }
    else {
        m_main_camera_pass->prepare(nullptr);
    }
    static_cast<MainCameraPass*>(m_main_camera_pass.get())->configShader(m_render_params.skybox, m_render_params.reflection, m_render_params.normal_debug, m_render_params.wireframe);
    static_cast<MainCameraPass*>(m_main_camera_pass.get())->configSamples(m_render_params.msaa_sample_count);
    static_cast<MainCameraPass*>(m_main_camera_pass.get())->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    m_main_camera_pass->draw();

    m_screen_pass->prepare(m_main_camera_pass->getFrameBuffer());
    m_screen_pass->draw();
}
