#ifndef WireFramePass_hpp
#define WireFramePass_hpp

#include "RenderPass.hpp"

class WireFramePass : public RenderPass
{
public:
	WireFramePass();
	void init() override;
	void draw() override;
};

#endif