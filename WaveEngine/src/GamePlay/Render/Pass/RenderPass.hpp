#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>
#include <memory>
#include "Platform/RHI/rhi.hpp"

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
	virtual void prepare(FrameBuffer* framebuffer) = 0;
	virtual void draw() = 0;
	virtual FrameBuffer* getFrameBuffer() = 0;
protected:
	std::unique_ptr<FrameBuffer> m_framebuffer;
};

#endif // !RenderPass_hpp

