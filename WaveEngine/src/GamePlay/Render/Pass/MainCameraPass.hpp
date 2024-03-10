#ifndef MainCameraPass_hpp
#define MainCameraPass_hpp

#include "../RenderPass.hpp"

class MainCameraPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void configShader(bool skybox, bool reflection, bool normal_debug, bool wireframe);
    void configSamples(int samples);
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    unsigned int m_shadow_map = 0;

    //params
    bool m_skybox = false;
    bool m_reflection = false;
    bool m_normal_debug = false;
    bool m_wireframe = false;
};

#endif
