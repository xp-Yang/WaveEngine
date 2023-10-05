#ifndef LightingPass_hpp
#define LightingPass_hpp

#include "RenderPass.hpp"

class LightingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_gbuffer_framebuffer{ nullptr };
    FrameBuffer* m_framebuffer{ nullptr };
    ScreenQuad* m_screen_quad;
};

#endif
