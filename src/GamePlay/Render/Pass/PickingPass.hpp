#ifndef PickingPass_hpp
#define PickingPass_hpp

#include "../RenderPass.hpp"

class PickingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    unsigned int m_fbo = 0;
};

#endif
