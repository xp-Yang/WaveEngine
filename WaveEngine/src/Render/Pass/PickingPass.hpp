#ifndef PickingPass_hpp
#define PickingPass_hpp

#include "RenderPass.hpp"

class PickingPass : public RenderPass {
public:
    void init() override;
    void draw() override;

private:
    unsigned int m_fbo = 0;
};

#endif
