#ifndef LightingPass_hpp
#define LightingPass_hpp

#include "../RenderPass.hpp"

class LightingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void prepare(FrameBuffer* g_fb, FrameBuffer* shadow_fb);
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_gbuffer_framebuffer{ nullptr };
    unsigned int m_shadow_map = 0;
    Mesh m_screen_quad;
};

#endif
