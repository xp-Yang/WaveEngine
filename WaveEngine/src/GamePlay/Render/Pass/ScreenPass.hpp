#ifndef ScreenPass_hpp
#define ScreenPass_hpp

#include "RenderPass.hpp"

class ScreenPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void draw() override;
    FrameBuffer* getFrameBuffer() override;

    // TODO
    void setPickView(FrameBuffer* frame_buffer);
    void setShadowView(FrameBuffer* frame_buffer);
    void setRayTracingView(FrameBuffer* frame_buffer);
private:
    FrameBuffer* m_pick_view_ref{ nullptr };
    FrameBuffer* m_rt_view_ref{ nullptr };
    FrameBuffer* m_shadow_view_ref{ nullptr };

private:
    std::unique_ptr<FrameBuffer> m_default_framebuffer{ nullptr };
    Mesh m_screen_quad;
};

#endif
