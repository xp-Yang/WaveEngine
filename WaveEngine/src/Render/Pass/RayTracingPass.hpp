#ifndef RayTracingPass_hpp
#define RayTracingPass_hpp

#include "RenderPass.hpp"

class RayTracingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;
};

#endif
