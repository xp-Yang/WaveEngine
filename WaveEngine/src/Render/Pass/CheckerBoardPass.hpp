#ifndef CheckerBoardPass_hpp
#define CheckerBoardPass_hpp

#include "RenderPass.hpp"

class CheckerBoardPass : public RenderPass
{
public:
	CheckerBoardPass();
	void init() override;
	void draw() override;
};

#endif