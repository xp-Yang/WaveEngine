#ifndef ScreenPass_hpp
#define ScreenPass_hpp

#include "../RenderPass.hpp"

class ScreenPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_default_framebuffer{ nullptr };
    FrameBuffer* m_framebuffer{ nullptr };
    ScreenQuad* m_screen_quad;
};

#endif
