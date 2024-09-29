#ifndef BrightPass_hpp
#define BrightPass_hpp

#include "RenderPass.hpp"

class BrightPass : public RenderPass
{
public:
	BrightPass();
	void init() override;
	void draw() override;
};

#endif