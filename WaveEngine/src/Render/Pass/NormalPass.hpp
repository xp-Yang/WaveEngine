#ifndef NormalPass_hpp
#define NormalPass_hpp

#include "RenderPass.hpp"

class NormalPass : public RenderPass
{
public:
	NormalPass();
	void draw() override;

protected:
	void init();
};

#endif