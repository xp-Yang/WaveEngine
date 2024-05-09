#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include "../Pass/RenderPass.hpp"

struct RenderParams {
    int     msaa_sample_count = 4;
    bool    skybox = false;
    bool    shadow = true;
    int     shadow_map_sample_count = 1;

    // for MainCameraPass
    bool    reflection = false;
    bool    normal_debug = false;
    bool    wireframe = false;
    bool    checkerboard = false;
    bool    pbr = true;
    //int     pixelate_level = 1;
};

// Interface class
class RenderPath {
public:
    enum class Type {
        Forward,
        Deferred,
        RayTracing,
    };
    virtual void init() = 0;
    virtual void prepareScreenQuadData(const std::shared_ptr<RenderSubMeshData>& screen_quad_data) = 0;
    virtual void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) = 0;
    virtual void render() = 0;
    void setRenderParams(const RenderParams& params) {
        m_render_params = params;
    }

protected:
    RenderParams m_render_params;
};

#endif // !RenderPipeline_hpp
