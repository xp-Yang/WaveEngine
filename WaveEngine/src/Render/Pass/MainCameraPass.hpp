#ifndef MainCameraPass_hpp
#define MainCameraPass_hpp

#include "RenderPass.hpp"

class MainCameraPass : public RenderPass {
public:
    void init() override;
    void configShader(bool skybox, bool reflection, bool normal_debug, bool wireframe);
    void configSamples(int samples);
    void draw() override;
    void setCubeMaps(const std::vector<unsigned int>& cube_maps) { m_cube_maps = cube_maps; }

private:
    unsigned int m_shadow_map = 0;
    std::vector<unsigned int> m_cube_maps;

    //params
    bool m_skybox = false;
    bool m_reflection = false;
    bool m_normal_debug = false;
    bool m_wireframe = false;
};

#endif
