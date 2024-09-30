#ifndef BloomPass_hpp
#define BloomPass_hpp

#include "RenderPass.hpp"

class BloomPass : public RenderPass
{
public:
	BloomPass();
	void init() override;
	void draw() override;

protected:
	void extractBright();
	void blur();

	std::unique_ptr<RhiFrameBuffer> m_pingpong_framebuffer_1;
	std::unique_ptr<RhiFrameBuffer> m_pingpong_framebuffer_2;
};

#endif