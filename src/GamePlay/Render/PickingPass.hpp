#ifndef PickingPass_hpp
#define PickingPass_hpp

#include "RenderPass.hpp"

class PickingPass : public RenderPass {
public:
    void init();
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();
};

#endif
