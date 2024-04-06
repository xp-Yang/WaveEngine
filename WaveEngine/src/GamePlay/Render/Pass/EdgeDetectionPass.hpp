#ifndef EdgeDetectionPass_hpp
#define EdgeDetectionPass_hpp

#include "RenderPass.hpp"

class EdgeDetectionPass : public RenderPass
{
public:
	EdgeDetectionPass();
	void init() override;
	void prepare(FrameBuffer* framebuffer) override;
	void draw() override;
	FrameBuffer* getFrameBuffer() override;

protected:
	Mesh m_screen_quad;

	std::unique_ptr<FrameBuffer> m_source_framebuffer;
	unsigned int m_source_map;
};

#endif