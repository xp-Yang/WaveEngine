#ifndef OutlinePass_hpp
#define OutlinePass_hpp

#include "RenderPass.hpp"

class OutlinePass : public RenderPass
{
public:
	OutlinePass();
	void draw() override;

protected:
	void init() override;

protected:
	std::unique_ptr<RhiFrameBuffer> m_source_framebuffer;
};

#endif