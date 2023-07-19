#include "MyCamera.hpp"

static glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
static const float MovementSpeed = 0.1f;

MyCamera::MyCamera(const glm::vec3& position, const glm::vec3& target)
    : m_pos (position)
    , m_target (target)
{
    m_direction = m_pos - m_target;
    m_view_matrix = glm::lookAt(m_pos, /*m_target*/m_pos + glm::vec3(0.0f, 0.0f, -1.0f), up);
}

void MyCamera::surround_with_target(const float radius) {
    //float camX = sin(glfwGetTime()) * radius;
    //float camZ = cos(glfwGetTime()) * radius;
    //m_view_matrix = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void MyCamera::key_process(int key)
{
    switch (key) {
    case GLFW_KEY_W:
        m_pos -= glm::vec3(0.0f, 0.0f, 1.0f) * MovementSpeed;
        break;
    case GLFW_KEY_A:
        m_pos += glm::vec3(1.0f, 0.0f, 0.0f) * MovementSpeed;
        break;
    case GLFW_KEY_D:
        m_pos -= glm::vec3(1.0f, 0.0f, 0.0f) * MovementSpeed;
        break;
    case GLFW_KEY_S:
        m_pos += glm::vec3(0.0f, 0.0f, 1.0f) * MovementSpeed;
        break;
    case GLFW_KEY_Z:
        m_pos -= glm::vec3(0.0f, 1.0f, 0.0f) * MovementSpeed;
        break;
    case GLFW_KEY_C:
        m_pos += glm::vec3(0.0f, 1.0f, 0.0f) * MovementSpeed;
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

    m_view_matrix = glm::lookAt(m_pos, /*m_target*/m_pos + glm::vec3(0.0f, 0.0f, -1.0f), up);

    std::cout << key << std::endl;
    printf("%.3f %.3f %.3f %.3f \n", m_view_matrix[0].x, m_view_matrix[1].x, m_view_matrix[2].x, m_view_matrix[3].x);
    printf("%.3f %.3f %.3f %.3f \n", m_view_matrix[0].y, m_view_matrix[1].y, m_view_matrix[2].y, m_view_matrix[3].y);
    printf("%.3f %.3f %.3f %.3f \n", m_view_matrix[0].z, m_view_matrix[1].z, m_view_matrix[2].z, m_view_matrix[3].z);
    printf("%.3f %.3f %.3f %.3f \n", m_view_matrix[0].w, m_view_matrix[1].w, m_view_matrix[2].w, m_view_matrix[3].w);
    std::cout << std::endl;
}

void MyCamera::set_view(const glm::mat4& view_matrix) {
    m_view_matrix = view_matrix;
}

const glm::vec3& MyCamera::get_position() {
    return m_pos;
}

const glm::vec3& MyCamera::get_target() {
    return m_target;
}

const glm::vec3& MyCamera::get_direction() {
    return m_direction;
}

const glm::vec3& MyCamera::get_right_direction() {
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, get_direction()));
    return cameraRight;
}

const glm::vec3& MyCamera::get_up_direction() {
    glm::vec3 cameraUp = glm::cross(m_direction, get_right_direction());
    return cameraUp;
}

const glm::mat4& MyCamera::get_view() {
    return m_view_matrix;
}