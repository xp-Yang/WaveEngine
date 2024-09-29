#ifndef NormalPass_hpp
#define NormalPass_hpp

#include "RenderPass.hpp"

class NormalPass : public RenderPass
{
public:
	NormalPass();
	void init() override;
	void draw() override;
};

#endif