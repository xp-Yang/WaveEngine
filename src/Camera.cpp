#include "Camera.hpp"

static const float CameraMovementSpeed = 20.0f;
static const float Sensitivity = 0.01f;
static const float ZoomUnit = 0.1f;

Camera::Camera(const glm::vec3& position, float pitch, float yaw, float roll) // pitch, yaw, roll angles are degree
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
    //���ĵ�һ������������fov��ֵ�������Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
    //�ڶ������������˿�߱ȣ����ӿڵĿ���Ը����á���߱�Ϊ1�Ļ����ӿڵĿ�߱�Ӱ������Ⱦ������������Ŀ�߱ȡ�
    //�����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ�档����ͨ�����ý�����Ϊ0.1f����Զ������Ϊ100.0f�������ڽ�ƽ���Զƽ�����Ҵ���ƽ��ͷ���ڵĶ��㶼�ᱻ��Ⱦ��
}

Camera::Camera(const glm::vec3& position, glm::vec3 dir)
    : m_pos(position)
{
    //m_direction.pitch = pitch;
    //m_direction.yaw = yaw;
    //m_direction.roll = roll;

    m_direction.dir = glm::normalize(dir);

    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);

    m_projection_matrix = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
}

void Camera::key_process(int key, float frame_time)
{
    // ÿһ֡����ʱ��Խ������ζ����һ֡����Ⱦ������Խ��ʱ�䣬������һ֡���ٶ�Ӧ��Խ����ƽ����Ⱦ����ȥ��ʱ��
    float frame_speed = CameraMovementSpeed * frame_time;
    auto forward = m_direction.dir;
    auto right = m_direction.get_right_direction();
    auto up = m_direction.get_up_direction();
    switch (key) {
    case GLFW_KEY_W:
        m_pos += forward * frame_speed;
        break;
    case GLFW_KEY_A:
        m_pos -= right * frame_speed;
        break;
    case GLFW_KEY_D:
        m_pos += right * frame_speed;
        break;
    case GLFW_KEY_S:
        m_pos -= forward * frame_speed;
        break;
    case GLFW_KEY_Z:
        m_pos += up * frame_speed;
        break;
    case GLFW_KEY_C:
        m_pos -= up * frame_speed;
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
}


void Camera::mouse_process(double delta_x, double delta_y, int mouse_button)
{
    // FPS style
    //if (mouse_button == 0) {
    //    // get pitch
    //    m_direction.pitch += delta_y * Sensitivity;
    //    // get yaw
    //    m_direction.yaw += delta_x * Sensitivity;

    //    // make sure that when pitch is out of bounds, screen doesn't get flipped
    //    if (m_direction.pitch > 89.0f)
    //        m_direction.pitch = 89.0f;
    //    if (m_direction.pitch < -89.0f)
    //        m_direction.pitch = -89.0f;

    //    // update direction
    //    m_direction.dir.x = cos(glm::radians(m_direction.pitch)) * sin(glm::radians(m_direction.yaw));
    //    m_direction.dir.y = sin(glm::radians(m_direction.pitch));
    //    m_direction.dir.z = -cos(glm::radians(m_direction.pitch)) * cos(glm::radians(m_direction.yaw));
    //    m_direction.dir = glm::normalize(m_direction.dir);

    //    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);
    //}

    if (mouse_button == 0) {
        // TODO 1.����Ļ������ת 2.���rotate_x direction = (0,-1,0)����ʱ������
        auto rotate_z = glm::rotate(glm::mat4(1.0f), -(float)(0.3f * delta_x * Sensitivity), up);
        //auto translate_test = glm::translate(glm::mat4(1.0f), {10.0f, 0.0f, 0.0f});
        //static auto pos = m_pos;
        //m_pos = translate_test * rotate_z * glm::vec4(pos, 1.0f);
        m_pos = rotate_z * glm::vec4(m_pos, 1.0f);
        m_direction.dir = rotate_z * glm::vec4(m_direction.dir, 1.0f);
        m_direction.dir = glm::normalize(m_direction.dir);

        auto rotate_x = glm::rotate(glm::mat4(1.0f), (float)(0.3f * delta_y * Sensitivity), m_direction.get_right_direction());
        m_pos = rotate_x * glm::vec4(m_pos, 1.0f);
        m_direction.dir = rotate_x * glm::vec4(m_direction.dir, 1.0f);
        m_direction.dir = glm::normalize(m_direction.dir);

        m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction.dir, up);
    }
    else if (mouse_button == 1) {
        m_pos += -(float)(delta_x * Sensitivity) * m_direction.get_right_direction();
        m_pos += -(float)(delta_y * Sensitivity) * m_direction.get_up_direction();

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

const glm::vec3& Camera::get_position() const {
    return m_pos;
}

const glm::vec3& Camera::get_target() const {
    return m_target;
}

const Direction& Camera::get_direction() const {
    return m_direction;
}

const glm::mat4& Camera::get_view() const {
    return m_view_matrix;
}

const glm::mat4& Camera::get_projection() const {
    return m_projection_matrix;
}
