#ifndef BlurPass_hpp
#define BlurPass_hpp

#include "RenderPass.hpp"

class BlurPass : public RenderPass
{
public:
	BlurPass();
	void init() override;
	void draw() override;

protected:
	std::unique_ptr<RhiFrameBuffer> m_pingpong_framebuffer;
};

#endif