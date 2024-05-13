#ifndef LightingPass_hpp
#define LightingPass_hpp

#include "RenderPass.hpp"

class LightingPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void prepare(FrameBuffer* g_fb, FrameBuffer* shadow_fb);
    void draw() override;
    FrameBuffer* getFrameBuffer() override;
    unsigned int getSceneMap() const;
    unsigned int getBrightMap() const;
    void enableSkybox(bool enable);
    void enableNormal(bool enable);
    void enableWireframe(bool enable);
    void enableCheckerboard(bool enable);
    void enablePBR(bool enable);
    void setCubeMaps(const std::vector<unsigned int>& cube_maps) { m_cube_maps = cube_maps; }

protected:
    void drawNormalMode();
    void drawWireframeMode();
    void drawCheckerboardMode();

private:
    std::unique_ptr<FrameBuffer> m_lights_framebuffer;
    FrameBuffer* m_gbuffer_framebuffer{ nullptr };
    unsigned int m_shadow_map = 0;

    bool m_skybox = false;
    bool m_normal_debug = false;
    bool m_wireframe = false;
    bool m_checkerboard = false;
    bool m_pbr = false;

    std::vector<unsigned int> m_cube_maps;
};

#endif