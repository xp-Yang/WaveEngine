#ifndef RenderPipeline_hpp
#define RenderPipeline_hpp

#include "Core/Common.hpp"

class Rhi;
class RenderMeshData;
struct RenderSourceData;

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
    virtual void prepareRhi(const std::shared_ptr<Rhi>& rhi) = 0;
    virtual void prepareScreenQuadData(const std::shared_ptr<RenderMeshData>& screen_quad_data) = 0;
    virtual void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) = 0;
    virtual void render() = 0;
    virtual unsigned int getSceneTexture() { return 0; };
    virtual unsigned int getPickingTexture() { return 0; };
    virtual unsigned int getPickingFBO() { return 0; };
    virtual unsigned int getShadowTexture() { return 0; };

protected:
    friend class RenderSystem;
    RenderParams m_render_params;
};

#endif // !RenderPipeline_hpp
