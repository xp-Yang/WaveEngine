#include "Camera.hpp"

static const float CameraMovementSpeed = 50.0f;
static const float Sensitivity = 0.1f;
static const float ZoomUnit = 0.1f;

std::string matrix_log(const glm::mat4 mat)
{
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].x, mat[1].x, mat[2].x, mat[3].x);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].y, mat[1].y, mat[2].y, mat[3].y);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].z, mat[1].z, mat[2].z, mat[3].z);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].w, mat[1].w, mat[2].w, mat[3].w);
    result += buf;
   //printf("%.3f %.3f %.3f %.3f \n", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
   //printf("%.3f %.3f %.3f %.3f \n", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
   //printf("%.3f %.3f %.3f %.3f \n", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
   //printf("%.3f %.3f %.3f %.3f \n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

std::string vec3_log(const glm::vec3 vec) {
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f \n", vec.x, vec.y, vec.z);
    result += buf;
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

Camera::Camera(const glm::vec3& position)
    : m_pos (position)
{
    m_direction.pitch = -45.0f;
    m_direction.yaw = 0.0f;
    m_direction.roll = 0.0f;

    m_direction.dir.x = cos(glm::radians(m_direction.pitch)) * sin(glm::radians(m_direction.yaw));
    m_direction.dir.y = sin(glm::radians(m_direction.pitch));
    m_direction.dir.z = -cos(glm::radians(m_direction.pitch)) * cos(glm::radians(m_direction.yaw));
    m_direction.dir = glm::normalize(m_direction.dir);

    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);

    m_projection_matrix = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    //它的第一个参数定义了fov的值。如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
    //第二个参数设置了宽高比，由视口的宽除以高所得。宽高比为1的话，视口的宽高比影响了渲染出来的立方体的宽高比。
    //第三和第四个参数设置了平截头体的近和远平面。我们通常设置近距离为0.1f，而远距离设为100.0f。所有在近平面和远平面内且处于平截头体内的顶点都会被渲染。
}

Camera::Camera(const glm::vec3& position, float pitch, float yaw, float roll)
    : m_pos(position)
{
    m_direction.pitch = pitch;
    m_direction.yaw = yaw;
    m_direction.roll = roll;

    m_direction.dir.x = cos(glm::radians(m_direction.pitch)) * sin(glm::radians(m_direction.yaw));
    m_direction.dir.y = sin(glm::radians(m_direction.pitch));
    m_direction.dir.z = -cos(glm::radians(m_direction.pitch)) * cos(glm::radians(m_direction.yaw));
    m_direction.dir = glm::normalize(m_direction.dir);

    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);

    m_projection_matrix = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
}

Camera::Camera(const glm::vec3& position, glm::vec3 dir)
{
    //m_direction.pitch = pitch;
    //m_direction.yaw = yaw;
    //m_direction.roll = roll;

    m_direction.dir = dir;

    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);

    m_projection_matrix = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
}


void Camera::surround_with_target(const float radius) {
    //float camX = sin(glfwGetTime()) * radius;
    //float camZ = cos(glfwGetTime()) * radius;
    //m_view_matrix = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void Camera::key_process(int key, float frame_time)
{
    // 每一帧持续时间越长，意味着上一帧的渲染花费了越多时间，所以这一帧的速度应该越大，来平衡渲染所花去的时间
    float frame_speed = CameraMovementSpeed * frame_time;

    switch (key) {
    case GLFW_KEY_W:
        m_pos -= glm::vec3(0.0f, 0.0f, 1.0f) * frame_speed;
        break;
    case GLFW_KEY_A:
        m_pos -= glm::vec3(1.0f, 0.0f, 0.0f) * frame_speed;
        break;
    case GLFW_KEY_D:
        m_pos += glm::vec3(1.0f, 0.0f, 0.0f) * frame_speed;
        break;
    case GLFW_KEY_S:
        m_pos += glm::vec3(0.0f, 0.0f, 1.0f) * frame_speed;
        break;
    case GLFW_KEY_Z:
        m_pos -= glm::vec3(0.0f, 1.0f, 0.0f) * frame_speed;
        break;
    case GLFW_KEY_C:
        m_pos += glm::vec3(0.0f, 1.0f, 0.0f) * frame_speed;
        break;
    case GLFW_KEY_1:
        break;
    case GLFW_KEY_2:
        break;
    case GLFW_KEY_3:
        break;
    case GLFW_KEY_4:
        break;
    case GLFW_KEY_SPACE:

        break;
    default:
        break;
    }

    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);

    std::cout << key << std::endl;
    matrix_log(m_view_matrix);
}


void Camera::mouse_process(double delta_x, double delta_y, int mouse_button)
{
    if (mouse_button == 0) {
        // get pitch
        m_direction.pitch += delta_y * Sensitivity;
        // get yaw
        m_direction.yaw += delta_x * Sensitivity;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_direction.pitch > 89.0f)
            m_direction.pitch = 89.0f;
        if (m_direction.pitch < -89.0f)
            m_direction.pitch = -89.0f;

        // update direction
        m_direction.dir.x = cos(glm::radians(m_direction.pitch)) * sin(glm::radians(m_direction.yaw));
        m_direction.dir.y = sin(glm::radians(m_direction.pitch));
        m_direction.dir.z = -cos(glm::radians(m_direction.pitch)) * cos(glm::radians(m_direction.yaw));
        m_direction.dir = glm::normalize(m_direction.dir);

        m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);
    }
    else if (mouse_button == 1) {
        m_pos += -(float)(delta_x * Sensitivity * 0.1f) * m_direction.get_right_direction();
        m_pos += -(float)(delta_y * Sensitivity * 0.1f) * m_direction.get_up_direction();

        m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);
    }
}

void Camera::mouse_scroll_process(double yoffset)
{
    m_zoom += ZoomUnit * yoffset;
    if (m_zoom < 0.1f)
        m_zoom = 0.1f;

    float fov = 45.0f;
    fov /= m_zoom;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 135.0f)
        fov = 135.0f;
    m_projection_matrix = glm::perspective(glm::radians(fov), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
}

const glm::vec3& Camera::get_position() {
    return m_pos;
}

const glm::vec3& Camera::get_target() {
    return m_target;
}

const Direction& Camera::get_direction() {
    return m_direction;
}

const glm::mat4& Camera::get_view() {
    return m_view_matrix;
}

const glm::mat4& Camera::get_projection() {
    return m_projection_matrix;
}
