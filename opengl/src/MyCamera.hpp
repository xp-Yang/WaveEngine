#ifndef MyCamera_hpp
#define MyCamera_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class CameraStyle {
    virtual void behavior() = 0;
};

class FPSStyle : CameraStyle {
    FPSStyle();
};

class FreeStyle : CameraStyle {

};

class StareStyle : CameraStyle {};

class MyCamera {
public:
    MyCamera(const glm::vec3& position, const glm::vec3& target);

    const glm::vec3& get_position();
    const glm::vec3& get_target();
    const glm::vec3& get_direction();
    const glm::vec3& get_right_direction();
    const glm::vec3& get_up_direction();
    const glm::mat4& get_view();

    void surround_with_target(const float radius);

    void key_process(int key, float frame_time);
    void mouse_process(double delta_x, double delta_y);
    void mouse_scroll_process(double yoffset);

private:
    CameraStyle* m_style{ nullptr };

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_direction;
    glm::mat4 m_view_matrix;

    float m_pitch = 0.0f;
    float m_yaw = -90.0f;
    float m_roll = 0.0f;

    float zoom;

};


#endif