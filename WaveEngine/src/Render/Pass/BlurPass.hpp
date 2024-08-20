#ifndef BlurPass_hpp
#define BlurPass_hpp

#include "RenderPass.hpp"

class BlurPass : public RenderPass
{
public:
	BlurPass();
	void init() override;
	void draw() override;
	unsigned int getBlurredBrightMap();
	void setBrightMap(unsigned int map);

protected:
	unsigned int m_bright_map;
	std::unique_ptr<RhiFrameBuffer> m_pingpong_framebuffer;
};

#endif