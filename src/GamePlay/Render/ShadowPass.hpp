#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    void init();
    void config(int shadow_map_sample_count);
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();

private:
    int m_shadow_map_sample_count = 1;
};

#endif
