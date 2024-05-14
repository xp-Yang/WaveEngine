#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "Path/RenderPath.hpp"
#include "Core/Signal/Signal.hpp"

class RenderSystem {
public:
    RenderSystem();
    RenderPath::Type getRenderPathType();
    void setRenderPathType(RenderPath::Type type);
    void setRenderParams(const RenderParams& params);
    void onUpdate();

public slots:
    void onComponentInserted(int entt_id, int pool_id);
    void onComponentRemoved(int entt_id, int pool_id);

protected:
    void updateRenderSourceData();

private:
    RenderPath::Type m_path_type;

    std::shared_ptr<RenderPath> m_forward_path;
    std::shared_ptr<RenderPath> m_deferred_path;
    std::shared_ptr<RenderPath> m_ray_tracing_path;

    std::shared_ptr<RenderPath> m_curr_path;

    Asset::SubMesh m_screen_quad_sub_mesh;
    std::shared_ptr<RenderSubMeshData> m_screen_quad;
    std::shared_ptr<RenderSourceData> m_render_source_data;
    bool m_initialized{ false };
    bool m_need_insert_source_data{ false };
    bool m_need_remove_source_data{ false };
    int m_need_insert_id{ -1 };
    int m_need_remove_id{ -1 };
};

#endif // !RenderSystem_hpp
