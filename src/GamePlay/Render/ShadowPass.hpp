#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void config() override;
    void config_samples(int samples) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_framebuffer{ nullptr };
};

#endif
