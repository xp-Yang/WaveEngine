#ifndef MainCameraPass_hpp
#define MainCameraPass_hpp

#include "RenderPass.hpp"

class MainCameraPass : public RenderPass {
public:
    void init();
    void config(int msaa_sample_count, bool reflection, bool normal_debug, bool wireframe);
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();

private:
    unsigned int m_rbo;
    unsigned int m_shadow_map = -1;
    int m_msaa_sample_count = 1;
    bool m_reflection = false;
    bool m_normal_debug = false;
    bool m_wireframe = false;
};

#endif
