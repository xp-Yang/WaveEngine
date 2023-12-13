#include "CameraSystem.hpp"
#include "World.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>

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
    // Viewing Style 转方向，并且相机位置也转动，聚焦于(0, 0, 0)点
	auto& world = ecs::World::get();
    ecs::CameraComponent* p_camera = nullptr;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        p_camera = world.getComponent<ecs::CameraComponent>(entity);
    }
    if (!p_camera)
        return;

    ecs::CameraComponent& camera = *p_camera;
    if (camera.mode == ecs::CameraComponent::Mode::Orbit) {
        if (mouse_button == 0) {
            auto rotate_Y = glm::rotate(glm::mat4(1.0f), -(float)(0.3f * delta_x * ecs::CameraComponent::Sensitivity), ecs::CameraComponent::up);
            camera.pos = rotate_Y * glm::vec4(camera.pos, 1.0f);
            camera.direction = rotate_Y * glm::vec4(camera.direction, 1.0f);
            camera.direction = glm::normalize(camera.direction);

            auto camera_right = camera.getRightDirection();
            auto rotate_x = glm::rotate(glm::mat4(1.0f), (float)(0.3f * delta_y * ecs::CameraComponent::Sensitivity), camera_right);
            camera.pos = rotate_x * glm::vec4(camera.pos, 1.0f);
            camera.direction = rotate_x * glm::vec4(camera.direction, 1.0f);
            camera.camera_up = glm::vec3(rotate_x * glm::vec4(camera.camera_up, 1.0f));

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
        }
        else if (mouse_button == 1) {
            camera.pos += -(float)(delta_x * ecs::CameraComponent::Sensitivity) * camera.getRightDirection();
            camera.pos += -(float)(delta_y * ecs::CameraComponent::Sensitivity) * camera.getUpDirection();

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
        }
    }

    if (camera.mode == ecs::CameraComponent::Mode::FPS) {
        // FPS style 自己不动，只转方向
        if (mouse_button == 0) {
            // get pitch
            camera.fps_params.pitch += delta_y * ecs::CameraComponent::Sensitivity;
            // get yaw
            camera.fps_params.yaw += delta_x * ecs::CameraComponent::Sensitivity;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (camera.fps_params.pitch > 89.0f)
                camera.fps_params.pitch = 89.0f;
            if (camera.fps_params.pitch < -89.0f)
                camera.fps_params.pitch = -89.0f;

            // update direction
            camera.direction.x = cos(glm::radians(camera.fps_params.pitch)) * sin(glm::radians(camera.fps_params.yaw));
            camera.direction.y = sin(glm::radians(camera.fps_params.pitch));
            camera.direction.z = -cos(glm::radians(camera.fps_params.pitch)) * cos(glm::radians(camera.fps_params.yaw));
            camera.direction = glm::normalize(camera.direction);

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
        }
    }
}

void CameraSystem::orbitRotate(Vec3 start, Vec3 end)
{
    auto& world = ecs::World::get();
    ecs::CameraComponent* p_camera = nullptr;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        p_camera = world.getComponent<ecs::CameraComponent>(entity);
    }
    if (!p_camera)
        return;
    ecs::CameraComponent& camera = *p_camera;

    // 计算旋转角度角度
    float angle = acos(fmin(1.0f, glm::dot(start, end)));
    // 计算旋转轴
    glm::vec3 rotate_axis = glm::normalize(glm::cross(start, end));
    //glm::vec3 world_rotate_axis = glm::inverse(glm::mat3(camera.view)) * rotate_axis;
    
    // 用四元数表示旋转
    //glm::quat rotateMat = glm::quat(cos(angle / 2),
    //    world_rotate_axis.x * sin(angle / 2),
    //    world_rotate_axis.y * sin(angle / 2),
    //    world_rotate_axis.z * sin(angle / 2));
    
    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angle, rotate_axis);

    camera.pos = rotate_mat * glm::vec4(camera.pos, 1.0f);
    camera.direction = rotate_mat * glm::vec4(camera.direction, 1.0f);
    camera.camera_up = glm::vec3(rotate_mat * glm::vec4(camera.camera_up, 1.0f));
    camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
}

void CameraSystem::onMouseWheelUpdate(double yoffset)
{
	auto& world = ecs::World::get();
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f;

        camera.fov = camera.originFov / camera.zoom;
        if (camera.fov <= glm::radians(1.0f))
            camera.fov = glm::radians(1.0f);
        if (camera.fov >= glm::radians(135.0f))
            camera.fov = glm::radians(135.0f);
        camera.projection = glm::perspective(camera.fov, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    }
}

}