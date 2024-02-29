#include "CameraSystem.hpp"
#include "World.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>
#include <Application_impl.hpp>

#include <imgui/imgui.h>
#include <Core/Logger.hpp>

namespace ecs {

void CameraSystem::onKeyUpdate(int key, float frame_time)
{
	auto& world = ecs::World::get();
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
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

void CameraSystem::onMouseUpdate(double delta_x, double delta_y, int mouse_button)
{
    // Viewing Style 转方向，并且相机位置也转动，聚焦于(0, 0, 0)点
	auto& world = ecs::World::get();
    ecs::CameraComponent* p_camera = world.getMainCameraComponent();
    if (!p_camera)
        return;

    ecs::CameraComponent& camera = *p_camera;
    if (camera.mode == ecs::CameraComponent::Mode::Orbit) {
        if (mouse_button == 0) {
            auto rotate_Y = glm::rotate(Mat4(1.0f), -(float)(0.3f * delta_x * ecs::CameraComponent::Sensitivity), ecs::CameraComponent::global_up);
            camera.pos = rotate_Y * Vec4(camera.pos, 1.0f);
            camera.direction = rotate_Y * Vec4(camera.direction, 1.0f);
            camera.direction = normalize(camera.direction);
            camera.camera_up = Vec3(rotate_Y * Vec4(camera.camera_up, 1.0f));

            auto camera_right = camera.getRightDirection();
            auto rotate_x = glm::rotate(Mat4(1.0f), (float)(0.3f * delta_y * ecs::CameraComponent::Sensitivity), camera_right);
            camera.pos = rotate_x * Vec4(camera.pos, 1.0f);
            camera.direction = rotate_x * Vec4(camera.direction, 1.0f);
            camera.camera_up = Vec3(rotate_x * Vec4(camera.camera_up, 1.0f));

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
            // 使用欧拉角导致的问题：在极点发生死锁，yaw和roll重合，失去了一个自由度
            if (camera.fps_params.pitch > 89.0f)
                camera.fps_params.pitch = 89.0f;
            if (camera.fps_params.pitch < -89.0f)
                camera.fps_params.pitch = -89.0f;

            // update direction
            camera.direction.x = cos(deg2rad(camera.fps_params.pitch)) * sin(deg2rad(camera.fps_params.yaw));
            camera.direction.y = sin(deg2rad(camera.fps_params.pitch));
            camera.direction.z = -cos(deg2rad(camera.fps_params.pitch)) * cos(deg2rad(camera.fps_params.yaw));
            
            camera.camera_up.x = sin(deg2rad(camera.fps_params.pitch)) * sin(deg2rad(camera.fps_params.yaw));
            camera.camera_up.y = cos(deg2rad(camera.fps_params.pitch));
            camera.camera_up.z = -sin(deg2rad(camera.fps_params.pitch)) * cos(deg2rad(camera.fps_params.yaw));

            camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
        }
    }
}

void CameraSystem::orbitRotate(Vec3 start, Vec3 end)
{
    auto& world = ecs::World::get();
    ecs::CameraComponent* p_camera = world.getMainCameraComponent();
    if (!p_camera)
        return;
    ecs::CameraComponent& camera = *p_camera;

    // 计算旋转角度角度
    float angle = acos(fmin(1.0f, dot(start, end)));
    // 计算旋转轴
    Vec3 rotate_axis = normalize(cross(start, end));
    //Vec3 world_rotate_axis = glm::inverse(Mat3(camera.view)) * rotate_axis;
    
    // 用四元数表示旋转
    //glm::quat rotateMat = glm::quat(cos(angle / 2),
    //    world_rotate_axis.x * sin(angle / 2),
    //    world_rotate_axis.y * sin(angle / 2),
    //    world_rotate_axis.z * sin(angle / 2));
    
    Mat4 rotate_mat = glm::rotate(Mat4(1.0f), angle, rotate_axis);

    camera.pos = rotate_mat * Vec4(camera.pos, 1.0f);
    camera.direction = rotate_mat * Vec4(camera.direction, 1.0f);
    camera.camera_up = Vec3(rotate_mat * Vec4(camera.camera_up, 1.0f));
    camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
}

void CameraSystem::onMouseWheelUpdate(double yoffset)
{
	auto& world = ecs::World::get();
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
    if (camera.zoom < 0.1f)
        camera.zoom = 0.1f;

    camera.fov = camera.originFov / camera.zoom;
    if (camera.fov <= deg2rad(1.0f))
        camera.fov = deg2rad(1.0f);
    if (camera.fov >= deg2rad(135.0f))
        camera.fov = deg2rad(135.0f);
    camera.projection = glm::perspective(camera.fov, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
}

void CameraSystem::onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y)
{
    auto& world = ecs::World::get();
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    if (camera.zoom_mode == ecs::CameraComponent::ZoomMode::ZoomToCenter) {
        onMouseWheelUpdate(yoffset);
    }
    if (camera.zoom_mode == ecs::CameraComponent::ZoomMode::ZoomToMouse) {
        auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
        main_viewport.transToScreenCoordinates();
        mouse_x -= main_viewport.x;
        mouse_y -= main_viewport.y;
        Vec3 mouse_3d_pos = rayCastPlaneZero(mouse_x, mouse_y);

        ImGui::Begin("Mouse Ray", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::NewLine();
        ImGui::Text("mouse 2d position:");
        std::string test_moue_2d_pos = vec3_log(Vec3(mouse_x, mouse_y, 0));
        ImGui::Text(test_moue_2d_pos.c_str());
        ImGui::NewLine();
        ImGui::Text("mouse 3d position:");
        std::string test_moue_3d_pos = vec3_log(mouse_3d_pos);
        ImGui::Text(test_moue_3d_pos.c_str());
        ImGui::End();

        float viewport_width = (float)main_viewport.width;
        float viewport_height = (float)main_viewport.height;
        Vec3 center_3d_pos = rayCastPlaneZero(viewport_width / 2.0f, viewport_height / 2.0f);
        Vec3 displacement = mouse_3d_pos - center_3d_pos;

        if (yoffset == 0.0)
            return;
        // 1. first translate to mouse_3d_pos
        camera.pos += displacement;
        float old_zoom = camera.zoom;

        // 2. set zoom
        camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f;

        camera.fov = camera.originFov / camera.zoom;
        if (camera.fov <= deg2rad(1.0f))
            camera.fov = deg2rad(1.0f);
        if (camera.fov >= deg2rad(135.0f))
            camera.fov = deg2rad(135.0f);

        // 3. second translate back to original pos
        //camera.pos -= displacement * (old_zoom / camera.zoom);

        // 4. set view matrix, projection matrix
        camera.view = glm::lookAt(camera.pos, camera.pos + camera.direction, camera.camera_up);
        camera.projection = glm::perspective(camera.fov, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    }
}

Vec3 CameraSystem::rayCastPlaneZero(double mouse_x, double mouse_y)
{
    auto& world = ecs::World::get();
    ecs::CameraComponent& camera = *world.getMainCameraComponent();

    // 1.  get ray direction from mouse position
    Vec3 cam_right = camera.getRightDirection();
    auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
    float viewport_width = (float)main_viewport.width;
    float viewport_height = (float)main_viewport.height;
    // normalized the x, y coordinate and take the viewport center as origin
    float u = 2.0f * mouse_x / viewport_width - 1.0f;
    float v = 2.0f * mouse_y / viewport_height - 1.0f;
    v = -v;

    float tangent = glm::tan(camera.fov / 2.0f);
    Vec3 ray_direction = camera.direction + cam_right * tangent * u * (main_viewport.AspectRatio()) + camera.camera_up * tangent * v;
    ray_direction = normalize(ray_direction);
    // 2.  solve the intersection equation of the ray and the plane: 
    // plane_normal. dot(m_position + t * ray_direction - p0) = 0 
    //`Vec3 plane_normal = Vec3(0, 1, 0);
    Vec3 plane_normal = -camera.direction;
    Vec4 zero_plane = Vec4(plane_normal.x, plane_normal.y, plane_normal.z, 0);
    Vec3 p0 = plane_normal * zero_plane[3];
    float t = (dot(plane_normal, p0) - dot(plane_normal, camera.pos) / dot(plane_normal, ray_direction));
    return Vec3(camera.pos + t * ray_direction);
}

}