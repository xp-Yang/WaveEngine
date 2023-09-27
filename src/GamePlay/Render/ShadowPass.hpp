#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void configSamples(int samples) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_framebuffer{ nullptr };
    FrameBuffer* m_framebuffer_undersampled{ nullptr };
};

#endif
