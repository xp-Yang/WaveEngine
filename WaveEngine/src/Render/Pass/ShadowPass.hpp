#ifndef ShadowPass_hpp 
#define ShadowPass_hpp

#include "RenderPass.hpp"

class ShadowPass : public RenderPass {
public:
    ShadowPass();
    void draw() override;
    void clear() override;
    const std::vector<unsigned int>& getCubeMaps() const { return m_cube_maps; }

protected:
    void init() override;
    void drawDirectionalLightShadowMap();
    void drawPointLightShadowMap();

    void reinit_cube_maps(size_t count);

private:
    std::vector<unsigned int> m_cube_maps;
    unsigned int m_cube_map_fbo = 0;
};

#endif
