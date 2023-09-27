#ifndef GBufferPass_hpp
#define GBufferPass_hpp

#include "RenderPass.hpp"

class GBufferPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void config_samples(int samples) override {}
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

private:
    FrameBuffer* m_frame_buffer{ nullptr };
    unsigned int m_position_map;
    unsigned int m_normal_map;
    unsigned int m_color_spec_map;
};

#endif // !GBufferPass_hpp
