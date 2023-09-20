#include "CameraSystem.hpp"
#include "World.hpp"
#include "Components.hpp"

namespace ecs {

void CameraSystem::onKeyUpdate(int key, float frame_time)
{
	auto& world = ecs::World::get();
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);

        // 每一帧持续时间越长，意味着上一帧的渲染花费了越多时间，所以这一帧的速度应该越大，来平衡渲染所花去的时间
        float frame_speed = ecs::CameraComponent::CameraMovementSpeed * frame_time;
		auto camera_forward = camera.direction;
		auto camera_right = camera.getRightDirection();
		auto camera_up = camera.getUpDirection();
        switch (key) {
        case GLFW_KEY_W:
            camera.pos += camera_forward * frame_speed;
            break;
        case GLFW_KEY_A:
            camera.pos -= camera_right * frame_speed;
            break;
        case GLFW_KEY_D:
            camera.pos += camera_right * frame_speed;
            break;
        case GLFW_KEY_S:
            camera.pos -= camera_forward * frame_speed;
            break;
        case GLFW_KEY_Z:
            camera.pos += camera_up * frame_speed;
            break;
        case GLFW_KEY_C:
            camera.pos -= camera_up * frame_speed;
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
        camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera_up);
    }

}

void CameraSystem::onMouseUpdate(double delta_x, double delta_y, int mouse_button)
{
	auto& world = ecs::World::get();
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        if (mouse_button == 0) {
            // TODO 1.绕屏幕中心旋转 2.解决rotate_x direction = (0,-1,0)朝向时的跳变
            auto rotate_z = glm::rotate(glm::mat4(1.0f), -(float)(0.3f * delta_x * ecs::CameraComponent::Sensitivity), ecs::CameraComponent::up);
            //auto translate_test = glm::translate(glm::mat4(1.0f), {10.0f, 0.0f, 0.0f});
            //static auto pos = camera.pos;
            //camera.pos = translate_test * rotate_z * glm::vec4(pos, 1.0f);
            camera.pos = rotate_z * glm::vec4(camera.pos, 1.0f);
            camera.direction = rotate_z * glm::vec4(camera.direction, 1.0f);
            camera.direction = glm::normalize(camera.direction);

            auto camera_right = camera.getRightDirection();
            auto rotate_x = glm::rotate(glm::mat4(1.0f), (float)(0.3f * delta_y * ecs::CameraComponent::Sensitivity), camera_right);
            camera.pos = rotate_x * glm::vec4(camera.pos, 1.0f);
            camera.direction = rotate_x * glm::vec4(camera.direction, 1.0f);
            camera.direction = glm::normalize(camera.direction);

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, ecs::CameraComponent::up);
        }
        else if (mouse_button == 1) {
            camera.pos += -(float)(delta_x * ecs::CameraComponent::Sensitivity) * camera.getRightDirection();
            camera.pos += -(float)(delta_y * ecs::CameraComponent::Sensitivity) * camera.getUpDirection();

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, ecs::CameraComponent::up);
        }
    }

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
    //    m_direction.x = cos(glm::radians(m_direction.pitch)) * sin(glm::radians(m_direction.yaw));
    //    m_direction.y = sin(glm::radians(m_direction.pitch));
    //    m_direction.z = -cos(glm::radians(m_direction.pitch)) * cos(glm::radians(m_direction.yaw));
    //    m_direction = glm::normalize(m_direction);

    //    m_view_matrix = glm::lookAt(m_pos, m_pos + m_direction, up);
    //}
}

void CameraSystem::onMouseWheelUpdate(double yoffset)
{
	auto& world = ecs::World::get();
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f;

        float fov = 45.0f;
        fov /= camera.zoom;
        if (fov <= 1.0f)
            fov = 1.0f;
        if (fov >= 135.0f)
            fov = 135.0f;
        camera.projection = glm::perspective(glm::radians(fov), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    }
}

}