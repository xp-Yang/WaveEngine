#ifndef ScreenPass_hpp
#define ScreenPass_hpp

#include "RenderPass.hpp"

class ScreenPass : public RenderPass {
public:
    void init();
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();
private:
    unsigned int quad_vao;
};

#endif
