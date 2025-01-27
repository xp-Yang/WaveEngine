#include "GcodeRenderPath.hpp"

#include "../Pass/RayTracingPass.hpp"
#include "../Pass/CombinePass.hpp"
#include "../Gcode/GcodeViewerPass.hpp"
#include "../Gcode/GcodeViewerInstancingPass.hpp"

#include "Logical/Gcode/GcodeViewer.hpp"
#include "Logical/Gcode/GcodeViewerInstancing.hpp"

GcodeRenderPath::GcodeRenderPath()
{
    m_render_passes[RenderPass::Type::Forward] = std::make_unique<GcodeViewerPass>();
    //m_gcode_instancing_pass = std::make_unique<Instance::GcodeViewerInstancingPass>();
    m_render_passes[RenderPass::Type::Combined] = std::make_unique<CombinePass>();

    //m_gcode_viewer = std::make_shared<GcodeViewer>();
    //m_gcode_viewer_instancing = std::make_shared<Instance::GcodeViewerInstancing>();

    static_cast<GcodeViewerPass*>(m_render_passes[RenderPass::Type::Forward].get())->setGcodeViewer(m_gcode_viewer.get());
    connect(m_gcode_viewer.get(), &(m_gcode_viewer->loaded),
        static_cast<GcodeViewerPass*>(m_render_passes[RenderPass::Type::Forward].get()), &GcodeViewerPass::reload_mesh_data);
    //static_cast<Instance::GcodeViewerInstancingPass*>(m_gcode_instancing_pass.get())->setGcodeViewer(ref_render_system->gcodeViewerInstancing().get());
    //connect(ref_render_system->gcodeViewerInstancing().get(), &(ref_render_system->gcodeViewerInstancing()->loaded), static_cast<Instance::GcodeViewerInstancingPass*>(m_gcode_instancing_pass.get()), &Instance::GcodeViewerInstancingPass::reload_mesh_data);
}

void GcodeRenderPath::render()
{
    m_render_passes[RenderPass::Type::Forward]->draw();
    m_render_passes[RenderPass::Type::Combined]->setInputPasses({ m_render_passes[RenderPass::Type::Forward].get() });
    //m_gcode_instancing_pass->draw();
    //m_combine_pass->setInputPasses({ m_gcode_instancing_pass.get() });
}
