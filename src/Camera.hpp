#ifndef Camera_hpp
#define Camera_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define WINDOW_WIDTH 1600.0f
#define WINDOW_HEIGHT 900.0f

static glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

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
    glm::vec3 dir; // 方向向量, 即camera的 -z 轴
    float pitch = 0.0f; // 方向向量与世界坐标系 x-z 平面的夹角
    float yaw = 0.0f; // 方向向量在世界坐标系 x-z 平面的投影矢量相对世界坐标系 -z 轴的夹角
    float roll = 0.0f;

    const glm::vec3& get_right_direction() const { // camera 的 x 轴
        glm::vec3 cameraRight = glm::normalize(glm::cross(dir, up));
        for (int i = 0; i < 3; i++) {
            if (abs(cameraRight[i] - 1) < 0.00001f)
                cameraRight[i] = 1.0f;
        }
        return cameraRight;
    }

    const glm::vec3& get_up_direction() const { // camera 的 y 轴
        glm::vec3 right_dir = get_right_direction();
        glm::vec3 cameraUp = glm::cross(right_dir, dir);
        for (int i = 0; i < 3; i++) {
            if (abs(cameraUp[i] - 1) < 0.00001f)
                cameraUp[i] = 1.0f;
        }
        return cameraUp;
    }
};

class Camera {
public:
    Camera(const glm::vec3& position, float pitch, float yaw, float roll);
    Camera(const glm::vec3& position, glm::vec3 dir);

    const glm::vec3& get_position() const;
    const glm::vec3& get_target() const;
    const Direction& get_direction() const;
    const glm::mat4& get_view() const;
    const glm::mat4& get_projection() const;

    void key_process(int key, float frame_time);
    void mouse_process(double delta_x, double delta_y, int mouse_button);
    void mouse_scroll_process(double yoffset);

private:
    //CameraStyle m_style;

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;
    Direction m_direction;

    float m_zoom = 1.0f;

};


#endif