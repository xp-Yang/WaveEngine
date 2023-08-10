#ifndef MyCamera_hpp
#define MyCamera_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define WINDOW_WIDTH 1600.0f
#define WINDOW_HEIGHT 900.0f

std::string matrix_log(const glm::mat4 mat);

class CameraStyle {
    virtual void behavior() = 0;
};

class FPSStyle : CameraStyle {
    FPSStyle();
};

class FreeStyle : CameraStyle {

};

class ObserveStyle : CameraStyle {};

struct Direction {
    glm::vec3 dir;
    float pitch = 0.0f; // 方向向量与 x-z 平面的夹角
    float yaw = 0.0f; // 方向向量在 x-z 平面的投影矢量相对 -z 轴的夹角
    float roll = 0.0f;
};

class MyCamera {
public:
    MyCamera(const glm::vec3& position, const glm::vec3& target);

    const glm::vec3& get_position();
    const glm::vec3& get_target();
    const Direction& get_direction();
    const glm::vec3& get_right_direction();
    const glm::vec3& get_up_direction();
    const glm::mat4& get_view();
    const glm::mat4& get_projection();

    void surround_with_target(const float radius);

    void key_process(int key, float frame_time);
    void mouse_process(double delta_x, double delta_y);
    void mouse_scroll_process(double yoffset);

private:
    CameraStyle* m_style{ nullptr };

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;
    Direction m_direction;

    float m_zoom = 1.0f;

};


#endif