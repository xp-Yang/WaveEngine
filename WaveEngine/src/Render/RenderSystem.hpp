#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "Core/Signal/Signal.hpp"
#include "Path/RenderPath.hpp"

enum class RenderPathType {
    Forward,
    Deferred,
    RayTracing,
};

struct RenderParams {
    int     msaa_sample_count = 4;
    bool    skybox = false;
    bool    shadow = true;
    int     shadow_map_sample_count = 1;

    bool    reflection = false;
    bool    normal_debug = false;
    bool    bloom = false;
    bool    wireframe = false;
    bool    checkerboard = false;
    bool    pbr = true;
    bool    hdr = true;
    bool    gamma = true;
    //int     pixelate_level = 1;
};

class Scene;
class RenderSystem {
public:
    RenderSystem();
    void init(std::shared_ptr<Scene> scene);
    RenderPathType getRenderPathType();
    void setRenderPathType(RenderPathType type);
    RenderParams& renderParams();
    void onUpdate();
    unsigned int getPickingFBO();
    unsigned int renderPassTexture(RenderPass::Type render_pass_type);

public slots:
    void onComponentInserted(int entt_id, int pool_id);
    void onComponentRemoved(int entt_id, int pool_id);

protected:
    void updateRenderSourceData();

private:
    RenderPathType m_path_type;
    RenderParams m_render_params;

    std::shared_ptr<RenderPath> m_forward_path;
    std::shared_ptr<RenderPath> m_deferred_path;
    std::shared_ptr<RenderPath> m_ray_tracing_path;

    std::shared_ptr<RenderPath> m_curr_path;

    std::shared_ptr<RenderSourceData> m_render_source_data;
    bool m_initialized{ false };
    bool m_need_insert_source_data{ false };
    bool m_need_remove_source_data{ false };
    int m_need_insert_id{ -1 };
    int m_need_remove_id{ -1 };

    std::shared_ptr<Scene> m_scene;
};

#endif // !RenderSystem_hpp
