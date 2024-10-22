#ifndef GcodeViewerPass_hpp
#define GcodeViewerPass_hpp

#include "Logical/Gcode/GcodeViewer.hpp"
#include "Render/Pass/RenderPass.hpp"

class GcodeViewer;
class GcodeViewerPass : public RenderPass {
public:
	GcodeViewerPass();
	void init() override;
	void draw() override;
	void setGcodeViewer(GcodeViewer* gcode_viewer) { m_gcode_viewer = gcode_viewer; }
	void reload_mesh_data(std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount> gcode_meshes);

private:
	std::vector<unsigned int> m_VAOs;
	std::vector<unsigned int> m_VBOs;
	std::vector<unsigned int> m_IBOs;

	GcodeViewer* m_gcode_viewer{ nullptr };
};

#endif // !GcodeViewerPass.hpp
