#ifndef DeferredLightingPass_hpp
#define DeferredLightingPass_hpp

#include "RenderPass.hpp"

class DeferredLightingPass : public RenderPass {
public:
    DeferredLightingPass();
    void init() override;
    void draw() override;
    void enableSkybox(bool enable);
    void enablePBR(bool enable);
    void setCubeMaps(const std::vector<unsigned int>& cube_maps) { m_cube_maps = cube_maps; }

private:
    bool m_skybox = false;
    bool m_pbr = false;

    unsigned int m_dir_light_shadow_map = 0;
    std::vector<unsigned int> m_cube_maps;
};

#endif
