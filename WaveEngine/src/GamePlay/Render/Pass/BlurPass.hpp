#ifndef BlurPass_hpp
#define BlurPass_hpp

#include "RenderPass.hpp"

class BlurPass : public RenderPass
{
public:
	BlurPass();
	void init() override;
	void prepare(FrameBuffer* framebuffer) override;
	void draw() override;
	FrameBuffer* getFrameBuffer() override;
	unsigned int getBlurredBrightMap();
	void setBrightMap(unsigned int map);

protected:
	Mesh m_screen_quad;
	unsigned int m_bright_map;
	std::unique_ptr<FrameBuffer> m_pingpong_framebuffer;
};

#endif