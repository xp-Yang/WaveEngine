#ifndef RenderPass_hpp
#define RenderPass_hpp

#include "Platform/RHI/rhi.hpp" // TODO 解除外部暴露
#include "Render/RenderSourceData.hpp"
#include "WaveEngine/Application.hpp" // TODO 解除依赖
#include "WaveEngine/Window.hpp"

// Interface class
// each RenderPass corresponds to a framebuffer
// TODO perhaps we need subpass to render into different attachments of one framebuffer
// a subpass (now as a RenderPass) may contain multiple graphics-pipeline, and execute pipeline multiple times 
// a graphics-pipeline need a shader program and meshes to execute
class RenderPass {
public:
	RenderPass() = default;
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	virtual ~RenderPass() = default;
	virtual void init() = 0;
	void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) { m_render_source_data = render_source_data; }
	void prepareScreenQuadData(const std::shared_ptr<RenderSubMeshData>& screen_quad_data) { m_screen_quad = screen_quad_data; }
	virtual void prepare(FrameBuffer* framebuffer) = 0;
	virtual void draw() = 0;
	virtual FrameBuffer* getFrameBuffer() = 0;
protected:
	std::unique_ptr<FrameBuffer> m_framebuffer;
	std::shared_ptr<RenderSubMeshData> m_screen_quad;
	std::shared_ptr<RenderSourceData> m_render_source_data;
};

#endif // !RenderPass_hpp

