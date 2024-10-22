#include "ForwardRenderPath.hpp"

#include "../Pass/ShadowPass.hpp"
#include "../Pass/MeshForwardLightingPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Gcode/GcodeViewerPass.hpp"
#include "../Pass/CombinePass.hpp"

#include "../RenderSystem.hpp"

ForwardRenderPath::ForwardRenderPath(RenderSystem* render_system) {
    m_picking_pass = std::make_unique<PickingPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_main_camera_pass = std::make_unique<MeshForwardLightingPass>();
    m_gcode_pass = std::make_unique<GcodeViewerPass>();
    m_combine_pass = std::make_unique<CombinePass>();

    ref_render_system = render_system;
}

void ForwardRenderPath::init()
{
    m_picking_pass->init();
    m_shadow_pass->init();
    m_main_camera_pass->init();
    m_gcode_pass->init();
    m_combine_pass->init();

    static_cast<GcodeViewerPass*>(m_gcode_pass.get())->setGcodeViewer(ref_render_system->gcodeViewer().get());
    connect(ref_render_system->gcodeViewer().get(), &(ref_render_system->gcodeViewer()->loaded), static_cast<GcodeViewerPass*>(m_gcode_pass.get()), &GcodeViewerPass::reload_mesh_data);
}

void ForwardRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_main_camera_pass->prepareRenderSourceData(render_source_data);
    m_gcode_pass->prepareRenderSourceData(render_source_data);
    m_combine_pass->prepareRenderSourceData(render_source_data);
}

void ForwardRenderPath::render()
{
    auto render_params = ref_render_system->renderParams();

    m_picking_pass->draw();

    if (render_params.shadow) {
        static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(render_params.shadow_map_sample_count);
        m_shadow_pass->draw();
    }
    else {
        static_cast<ShadowPass*>(m_shadow_pass.get())->clear();
    }

    m_main_camera_pass->setInputPasses({ m_shadow_pass.get() });

    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->enablePBR(render_params.pbr);
    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->enableReflection(render_params.reflection);
    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->configSamples(render_params.msaa_sample_count);
    static_cast<MeshForwardLightingPass*>(m_main_camera_pass.get())->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
    m_main_camera_pass->draw();

    m_gcode_pass->setInputPasses({ m_main_camera_pass.get() }); // draw above the main light pass framebuffer
    m_gcode_pass->draw();

    m_combine_pass->setInputPasses({ m_main_camera_pass.get() });
    m_combine_pass->draw();
}

unsigned int ForwardRenderPath::getPickingFBO()
{
    return m_picking_pass->getFrameBuffer()->id();
}

RhiTexture* ForwardRenderPath::renderPassTexture(RenderPass::Type render_pass_type)
{
    if (render_pass_type == RenderPass::Type::Picking)
        return m_picking_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::Shadow)
        return m_shadow_pass->getFrameBuffer()->depthAttachment()->texture();
    if (render_pass_type == RenderPass::Type::Combined)
        return m_combine_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();

    return nullptr;
}