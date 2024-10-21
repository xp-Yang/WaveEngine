#ifndef GcodeViewerPass_hpp
#define GcodeViewerPass_hpp

#include "Render/Pass/RenderPass.hpp"

class GcodeViewerPass : public RenderPass {
public:
	GcodeViewerPass();
	void init() override;
	void draw() override;
};

#endif // !GcodeViewerPass.hpp
