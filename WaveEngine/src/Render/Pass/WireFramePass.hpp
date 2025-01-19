#ifndef WireFramePass_hpp
#define WireFramePass_hpp

#include "RenderPass.hpp"

class WireFramePass : public RenderPass
{
public:
	WireFramePass();
	void draw() override;

protected:
	void init();
};

#endif