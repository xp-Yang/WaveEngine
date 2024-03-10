#ifndef LightingPass_hpp
#define LightingPass_hpp

#include "../RenderPass.hpp"

class LightingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void prepare(FrameBuffer* g_fb, FrameBuffer* shadow_fb);
    void draw() override;
    FrameBuffer* getFrameBuffer() override;
    void enableNormal(bool enable);
    void enableWireframe(bool enable);
    void enableGrid(bool enable);

protected:
    void drawNormalMode();
    void drawWireframeMode();
    void drawGridMode();

private:
    FrameBuffer* m_gbuffer_framebuffer{ nullptr };
    unsigned int m_shadow_map = 0;
    Mesh m_screen_quad;

    bool m_normal_debug = false;
    bool m_wireframe = false;
    bool m_grid = false;
};

#endif
