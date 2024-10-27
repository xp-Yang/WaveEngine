#ifndef GcodeViewerPass_hpp
#define GcodeViewerPass_hpp

#include "Logical/Gcode/GcodeViewer.hpp"
#include "Render/Pass/RenderPass.hpp"

class GcodeViewerPass : public RenderPass {
public:
	void init() override;
	void draw() override;
	void setGcodeViewer(GcodeViewer* gcode_viewer) { m_gcode_viewer = gcode_viewer; }
	void reload_mesh_data(std::array<LinesBatch, ExtrusionRole::erCount> lines_batches);

private:
	std::vector<unsigned int> m_VAOs;
	std::vector<unsigned int> m_VBOs;
	std::vector<unsigned int> m_IBOs;
	std::vector<Color4> m_colors;

	GcodeViewer* m_gcode_viewer{ nullptr };
};

#endif // !GcodeViewerPass.hpp
