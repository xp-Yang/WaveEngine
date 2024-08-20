#ifndef RenderPass_hpp
#define RenderPass_hpp

#include "Render/RenderSourceData.hpp"
#include "Engine.hpp"
#include "Render/RHI/rhi.hpp"

// Interface class
// each RenderPass corresponds to a framebuffer
// there could be different attachments(render targets, like texture/render buffer object) in one framebuffer
// perhaps we need subpass, so that can use input attachments to render
// a subpass (now as a RenderPass) may contain multiple graphics-pipeline, and execute graphics-pipeline multiple times 
// a graphics-pipeline need shader program and vertices to execute
class RenderPass {
public:
	RenderPass() = default;
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	virtual ~RenderPass() = default;
	virtual void init() = 0;
	void prepareRhi(const std::shared_ptr<Rhi>& rhi) { m_rhi = rhi; }
	void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) { m_render_source_data = render_source_data; }
	void prepareScreenQuadData(const std::shared_ptr<RenderMeshData>& screen_quad_data) { m_screen_quad = screen_quad_data; }
	void setInputPasses(const std::vector<RenderPass*>& input_passes) { m_input_passes = input_passes; }
	RhiFrameBuffer* getFrameBuffer() const { return m_framebuffer.get(); };
	virtual void draw() = 0;
protected:
	std::shared_ptr<RenderMeshData> m_screen_quad;
	std::shared_ptr<RenderSourceData> m_render_source_data;
	std::shared_ptr<Rhi> m_rhi;

	std::vector<RenderPass*> m_input_passes;
	std::unique_ptr<RhiFrameBuffer> m_framebuffer;
};

#endif // !RenderPass_hpp

