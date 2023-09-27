#ifndef PickingPass_hpp
#define PickingPass_hpp

#include "RenderPass.hpp"

class PickingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void config_samples(int samples) override {}
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_framebuffer{ nullptr };
    unsigned int m_fbo;
};

#endif
