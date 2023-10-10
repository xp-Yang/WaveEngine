#ifndef GBufferPass_hpp
#define GBufferPass_hpp

#include "../RenderPass.hpp"

class GBufferPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_framebuffer{ nullptr };
};

#endif // !GBufferPass_hpp
