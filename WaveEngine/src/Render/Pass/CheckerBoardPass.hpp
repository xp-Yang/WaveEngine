#ifndef CheckerBoardPass_hpp
#define CheckerBoardPass_hpp

#include "RenderPass.hpp"

class CheckerBoardPass : public RenderPass
{
public:
	CheckerBoardPass();
	void draw() override;

protected:
	void init() override;
};

#endif