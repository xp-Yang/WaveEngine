#ifndef MainCameraPass_hpp
#define MainCameraPass_hpp

#include "RenderPass.hpp"

class MainCameraPass : public RenderPass {
public:
    void init();
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();
private:
    unsigned int shadow_map;
};

#endif
