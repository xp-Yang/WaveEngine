#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>
#include <memory>
#include "Platform/RHI/rhi.hpp"

// ½Ó¿ÚÀà
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

