#ifndef ScreenPass_hpp
#define ScreenPass_hpp

#include "RenderPass.hpp"

class ScreenPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void configSamples(int samples) override {}
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_framebuffer{ nullptr };

    class ScreenQuad {
    public:
        unsigned int create();
        unsigned int getVAO();
    private:
        unsigned int m_vao = 0;
    };
    ScreenQuad* m_screen_quad;
};

#endif
