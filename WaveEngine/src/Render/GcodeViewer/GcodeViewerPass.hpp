#ifndef GcodeViewerPass_hpp
#define GcodeViewerPass_hpp

#include "Render/Pass/RenderPass.hpp"

class GcodeViewerPass : public RenderPass {
public:
	GcodeViewerPass();
	void init() override;
	void draw() override;

private:
	std::vector<std::shared_ptr<RenderMeshData>> m_render_meshes;
	std::vector<std::shared_ptr<RenderMaterialData>> m_render_materials;
};

#endif // !GcodeViewerPass.hpp
