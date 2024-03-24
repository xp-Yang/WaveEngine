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

private:
    RenderPath::Type m_path_type;

    std::shared_ptr<RenderPath> m_forward_path;
    std::shared_ptr<RenderPath> m_deferred_path;
    std::shared_ptr<RenderPath> m_ray_tracing_path;

    std::shared_ptr<RenderPath> m_curr_path;
};

#endif // !RenderSystem_hpp
