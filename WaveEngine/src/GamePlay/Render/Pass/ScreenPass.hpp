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
    std::unique_ptr<FrameBuffer> m_default_framebuffer{ nullptr };
    Mesh m_screen_quad;
};

#endif
