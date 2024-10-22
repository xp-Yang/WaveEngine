#ifndef GcodeViewerPass_hpp
#define GcodeViewerPass_hpp

#include "Render/Pass/RenderPass.hpp"

class GcodeViewer;
class GcodeViewerPass : public RenderPass {
public:
	GcodeViewerPass();
	void init() override;
	void draw() override;
	void reload_mesh_data(std::vector<std::shared_ptr<Mesh>> gcode_meshes);

private:
	std::vector<unsigned int> m_VAOs;
	std::vector<unsigned int> m_VBOs;
	std::vector<unsigned int> m_IBOs;
};

#endif // !GcodeViewerPass.hpp
