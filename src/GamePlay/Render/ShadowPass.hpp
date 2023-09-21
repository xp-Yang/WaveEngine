#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    void init();
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();
};

#endif
