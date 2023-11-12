#ifndef View_hpp
#define View_hpp

class Scene;
class View {
public:
    View() = default;

    //void set_camera(Camera* camera) { this->camera = camera; } //View 并不拥有 Camera
    //const Camera& get_camera() const { return *camera; }
    //Camera& get_camera() { return *camera; }
    void set_scene(Scene* scene) { this->scene = scene; }    //View 并不拥有 Scene
    const Scene& get_scene() const { return *scene; }
    Scene& get_scene() { return *scene; }

    //void set_render_quality();
    //void set_render_style();
    //void enable_shadow_map(bool enable);
    //bool is_shadow_map_enable() const { return m_enable_shadow_map; }
    //void set_shadow_map_id(unsigned int id) { shadow_map_id = id; }
    //unsigned int get_shadow_map_id() const { return shadow_map_id; }


private:
    //Camera* camera{ nullptr };
    Scene* scene{ nullptr };

    //float render_parameters;
    //unsigned int shadow_map_id;
    //bool m_enable_shadow_map{ false };
};

#endif