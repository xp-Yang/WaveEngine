#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "Path/RenderPath.hpp"

class RenderSystem {
public:
    RenderSystem();
    RenderPath::Type getRenderPathType();
    void setRenderPathType(RenderPath::Type type);
    void setRenderParams(const RenderParams& params);
    void onUpdate();

protected:
    void updateRenderSourceData();

private:
    RenderPath::Type m_path_type;

    std::shared_ptr<RenderPath> m_forward_path;
    std::shared_ptr<RenderPath> m_deferred_path;
    std::shared_ptr<RenderPath> m_ray_tracing_path;

    std::shared_ptr<RenderPath> m_curr_path;

    std::shared_ptr<RenderSubMeshData> m_screen_quad;
    std::shared_ptr<RenderSourceData> m_render_source_data;
    bool m_need_process_source_data{ true };
};

#endif // !RenderSystem_hpp
