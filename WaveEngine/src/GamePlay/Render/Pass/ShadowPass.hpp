#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "../RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    void init() override;
    void prepare(FrameBuffer* framebuffer) override;
    void configSamples(int samples);
    void draw() override;
    void clear();
    FrameBuffer* getFrameBuffer() override;
    unsigned int getCubeMap() { return m_cube_map; }

protected:
    void drawDirectionalLightShadowMap();
    void drawPointLightShadowMap();

private:
    FrameBuffer* m_cube_map_framebuffer{ nullptr };
    unsigned int m_cube_map = 0;
    unsigned int m_cube_map_fbo = 0;
};

#endif
