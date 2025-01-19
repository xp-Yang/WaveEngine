#ifndef BloomPass_hpp
#define BloomPass_hpp

#include "RenderPass.hpp"

class BloomPass : public RenderPass
{
public:
	BloomPass();
	void draw() override;

protected:
	void init() override;
	void extractBright();
	void blur();

	std::unique_ptr<RhiFrameBuffer> m_pingpong_framebuffer;
};

#endif