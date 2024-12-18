#include "DeferredRenderPath.hpp"

#include "../Pass/ZPrePass.hpp"
#include "../Pass/ShadowPass.hpp"
#include "../Pass/WireFramePass.hpp"
#include "../Pass/CheckerBoardPass.hpp"
#include "../Pass/NormalPass.hpp"
#include "../Pass/GBufferPass.hpp"
#include "../Pass/DeferredLightingPass.hpp"
#include "../Pass/TransparentPass.hpp"
#include "../Pass/SkyBoxPass.hpp"
#include "../Pass/BloomPass.hpp"
#include "../Pass/PickingPass.hpp"
#include "../Pass/OutlinePass.hpp"
#include "../Pass/CombinePass.hpp"

#include "../RenderSystem.hpp"

DeferredRenderPath::DeferredRenderPath(RenderSystem* render_system)
{
    m_z_pre_pass = std::make_unique<ZPrePass>();
    m_picking_pass = std::make_unique<PickingPass>();
    m_wireframe_pass = std::make_unique<WireFramePass>();
    m_checkerboard_pass = std::make_unique<CheckerBoardPass>();
    m_normal_pass = std::make_unique<NormalPass>();
    m_shadow_pass = std::make_unique<ShadowPass>();
    m_gbuffer_pass = std::make_unique<GBufferPass>();
    m_lighting_pass = std::make_unique<DeferredLightingPass>();
    m_transparent_pass = std::make_unique<TransparentPass>();
    m_skybox_pass = std::make_unique<SkyBoxPass>();
    m_bloom_pass = std::make_unique<BloomPass>();
    m_outline_pass = std::make_unique<OutlinePass>();
    m_combine_pass = std::make_unique<CombinePass>();

    ref_render_system = render_system;
}

void DeferredRenderPath::init()
{
    m_z_pre_pass->init();
    m_picking_pass->init();
    m_wireframe_pass->init();
    m_checkerboard_pass->init();
    m_normal_pass->init();
    m_shadow_pass->init();
    m_gbuffer_pass->init();
    m_lighting_pass->init();
    m_transparent_pass->init();
    m_skybox_pass->init();
    m_bloom_pass->init();
    m_outline_pass->init();
    m_combine_pass->init();
}

void DeferredRenderPath::prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data)
{
    m_z_pre_pass->prepareRenderSourceData(render_source_data);
    m_picking_pass->prepareRenderSourceData(render_source_data);
    m_wireframe_pass->prepareRenderSourceData(render_source_data);
    m_checkerboard_pass->prepareRenderSourceData(render_source_data);
    m_normal_pass->prepareRenderSourceData(render_source_data);
    m_shadow_pass->prepareRenderSourceData(render_source_data);
    m_gbuffer_pass->prepareRenderSourceData(render_source_data);
    m_lighting_pass->prepareRenderSourceData(render_source_data);
    m_transparent_pass->prepareRenderSourceData(render_source_data);
    m_skybox_pass->prepareRenderSourceData(render_source_data);
    m_bloom_pass->prepareRenderSourceData(render_source_data);
    m_outline_pass->prepareRenderSourceData(render_source_data);
    m_combine_pass->prepareRenderSourceData(render_source_data);
}

void DeferredRenderPath::render()
{
    const auto& render_params = ref_render_system->renderParams();

    auto combine_pass = static_cast<CombinePass*>(m_combine_pass.get());

    //m_z_pre_pass->draw();

    m_picking_pass->draw();

    std::vector<RenderPass*> combine_input_passes;
    RenderPass* main_light_pass;

    if (render_params.checkerboard) {
        m_checkerboard_pass->draw();
        combine_input_passes = { m_checkerboard_pass.get()};
        main_light_pass = m_checkerboard_pass.get();
    }
    else {
        if (render_params.shadow) {
            //static_cast<ShadowPass*>(m_shadow_pass.get())->configSamples(render_params.shadow_map_sample_count);
            m_shadow_pass->draw();
        }
        else
            m_shadow_pass->clear();

        auto gbuffer_pass = static_cast<GBufferPass*>(m_gbuffer_pass.get());
        gbuffer_pass->enablePBR(render_params.pbr);
        gbuffer_pass->draw();

        auto lighting_pass = static_cast<DeferredLightingPass*>(m_lighting_pass.get());
        lighting_pass->enablePBR(render_params.pbr);
        lighting_pass->setCubeMaps(static_cast<ShadowPass*>(m_shadow_pass.get())->getCubeMaps());
        lighting_pass->setInputPasses({ m_gbuffer_pass.get(), m_shadow_pass.get() });
        lighting_pass->draw();

        if (render_params.skybox) {
            auto skybox_pass = static_cast<SkyBoxPass*>(m_skybox_pass.get());
            skybox_pass->setInputPasses({ m_lighting_pass.get() }); // draw above the lighting pass framebuffer
            skybox_pass->draw();
        }

        auto transparent_pass = static_cast<TransparentPass*>(m_transparent_pass.get());
        transparent_pass->setInputPasses({ m_lighting_pass.get() }); // draw above the lighting pass framebuffer
        transparent_pass->draw();

        if (render_params.bloom) {
            m_bloom_pass->setInputPasses({ m_lighting_pass.get() }); // need extract bright
            m_bloom_pass->draw();
        }
        else {
            m_bloom_pass->clear();
        }

        combine_input_passes = { m_lighting_pass.get(), m_bloom_pass.get() };
        main_light_pass = m_lighting_pass.get();
    }

    if (render_params.normal_debug) {
        m_normal_pass->setInputPasses({ main_light_pass }); // draw above the main light pass framebuffer
        m_normal_pass->draw();
    }
    if (render_params.wireframe) {
        m_wireframe_pass->setInputPasses({ main_light_pass }); // draw above the main light pass framebuffer
        m_wireframe_pass->draw();
    }

    m_outline_pass->setInputPasses({ main_light_pass }); // draw above the main light pass framebuffer
    m_outline_pass->draw();

    combine_pass->setInputPasses(combine_input_passes);
    combine_pass->draw();
}

RhiTexture* DeferredRenderPath::renderPassTexture(RenderPass::Type render_pass_type)
{
    if (render_pass_type == RenderPass::Type::Picking)
        return m_picking_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::Shadow)
        return m_shadow_pass->getFrameBuffer()->depthAttachment()->texture();
    if (render_pass_type == RenderPass::Type::GBuffer)
        return m_gbuffer_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::DeferredLighting)
        return m_lighting_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::Bloom)
        return m_bloom_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::Outline)
        return m_outline_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();
    if (render_pass_type == RenderPass::Type::Combined)
        return m_combine_pass->getFrameBuffer()->colorAttachmentAt(0)->texture();

    return nullptr;
}

unsigned int DeferredRenderPath::getPickingFBO()
{
    return m_picking_pass->getFrameBuffer()->id();
}
