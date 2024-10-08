#ifndef TransparentPass_hpp
#define TransparentPass_hpp

#include "RenderPass.hpp"

class TransparentPass : public RenderPass {
public:
    TransparentPass();
    void init() override;
    void draw() override;
};

#endif // !TransparentPass_hpp