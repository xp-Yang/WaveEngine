#ifndef View_hpp
#define View_hpp

#include "Camera.hpp"
#include "Scene.hpp"

struct RenderParam {
    //多重采样抗锯齿(MSAA , MultiSampling Anti-Aliasing)
    //阴影贴图(shadow mapping)
    //动态分辨率（dynamic resolution)
    //渲染质量等级(QualityLevel)
    //色调映射和伽马编码(Tone-mapping & gamma encoding)
    //抖动显示(Dithering)
    //快速近似抗锯齿(FXAA, Fast Approximate Anti-Aliasing)
    //picking
};

class View {
public:
    View() = default;

    void set_camera(Camera* camera) { this->camera = camera; } //View 并不拥有 Camera
    void set_scene(Scene* scene) { this->scene = scene; }    //View 并不拥有 Scene
    void set_view_port(int width, int height) { glViewport(0, 0, width, height); }
    const Camera& get_camera() const { return *camera; }
    Camera& get_camera() { return *camera; }
    const Scene& get_scene() const { return *scene; }
    Scene& get_scene() { return *scene; }

    void set_render_quality();
    void enable_shadow_map();
    void set_render_style();

private:
    Camera* camera{ nullptr };
    Scene* scene{ nullptr };

    float render_parameters;

};

#endif