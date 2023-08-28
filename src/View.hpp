#ifndef View_hpp
#define View_hpp

#include "Camera.hpp"
#include "Scene.hpp"

struct RenderParam {
    //���ز��������(MSAA , MultiSampling Anti-Aliasing)
    //��Ӱ��ͼ(shadow mapping)
    //��̬�ֱ��ʣ�dynamic resolution)
    //��Ⱦ�����ȼ�(QualityLevel)
    //ɫ��ӳ���٤�����(Tone-mapping & gamma encoding)
    //������ʾ(Dithering)
    //���ٽ��ƿ����(FXAA, Fast Approximate Anti-Aliasing)
    //picking
};

class View {
public:
    View() = default;

    void set_camera(Camera* camera) { this->camera = camera; } //View ����ӵ�� Camera
    void set_scene(Scene* scene) { this->scene = scene; }    //View ����ӵ�� Scene
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