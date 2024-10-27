#ifndef GcodeViewerInstancingPass_hpp
#define GcodeViewerInstancingPass_hpp

#include "Logical/Gcode/GcodeViewerInstancing.hpp"
#include "Render/Pass/RenderPass.hpp"

namespace Instance {
class GcodeViewerInstancingPass : public RenderPass {
public:
	void init() override;
	void draw() override;
	void setGcodeViewer(GcodeViewerInstancing* gcode_viewer) { m_gcode_viewer = gcode_viewer; }
	void reload_mesh_data(std::array<LinesBatch, ExtrusionRole::erCount> lines_batches);

private:
	std::vector<unsigned int> m_VAOs;
	std::vector<unsigned int> m_Instances;
	std::vector<Color4> m_colors;

	GcodeViewerInstancing* m_gcode_viewer{ nullptr };
};
}

#endif // !GcodeViewerInstancingPass.hpp
