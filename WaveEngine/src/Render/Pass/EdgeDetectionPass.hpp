#ifndef EdgeDetectionPass_hpp
#define EdgeDetectionPass_hpp

#include "RenderPass.hpp"

class EdgeDetectionPass : public RenderPass
{
public:
	EdgeDetectionPass();
	void init() override;
	void draw() override;

protected:
	std::unique_ptr<RhiFrameBuffer> m_source_framebuffer;
	unsigned int m_source_map;
};

#endif