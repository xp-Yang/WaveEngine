#include "CameraSystem.hpp"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include <Application_impl.hpp>
#include <Core/Logger.hpp>

namespace ecs {

CameraSystem::CameraSystem()
    : world(ecs::World::get())
    , main_camera(*ecs::World::get().getMainCameraComponent())
{
}

void CameraSystem::onUpdate()
{
    if (!m_need_update)
        return;
    
    main_camera.fov = lerp(main_camera.fov, m_goal_fov, 0.1f);
    main_camera.projection = Perspective(main_camera.fov, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

    if (isApproxZero(main_camera.fov - m_goal_fov))
        m_need_update = false;

}

void CameraSystem::onKeyUpdate(int key, float frame_time)
{
    // 每一帧持续时间越长，意味着上一帧的渲染花费了越多时间，所以这一帧的速度应该越大，来平衡渲染所花去的时间
    float frame_speed = ecs::CameraComponent::CameraMovementSpeed * frame_time;
	auto camera_forward = main_camera.direction;
	auto camera_right = main_camera.getRightDirection();
	auto camera_up = main_camera.getUpDirection();
    switch (key) {
    case GLFW_KEY_W:
        main_camera.pos += camera_forward * frame_speed;
        break;
    case GLFW_KEY_A:
        main_camera.pos -= camera_right * frame_speed;
        break;
    case GLFW_KEY_D:
        main_camera.pos += camera_right * frame_speed;
        break;
    case GLFW_KEY_S:
        main_camera.pos -= camera_forward * frame_speed;
        break;
    case GLFW_KEY_Z:
        main_camera.pos += camera_up * frame_speed;
        break;
    case GLFW_KEY_C:
        main_camera.pos -= camera_up * frame_speed;
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
    main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, camera_up);
}

void CameraSystem::onMouseUpdate(double delta_x, double delta_y, MouseButton mouse_button)
{
    // Viewing Style 转方向，并且相机位置也转动，聚焦于(0, 0, 0)点

    if (main_camera.mode == ecs::CameraComponent::Mode::Orbit) {
        if (mouse_button == MouseButton::Left) {
            auto rotate_Y = Rotate(-(float)(0.3f * delta_x * ecs::CameraComponent::Sensitivity), ecs::CameraComponent::global_up);
            main_camera.pos = rotate_Y * Vec4(main_camera.pos, 1.0f);
            main_camera.direction = rotate_Y * Vec4(main_camera.direction, 1.0f);
            main_camera.direction = Normalize(main_camera.direction);
            main_camera.camera_up = Vec3(rotate_Y * Vec4(main_camera.camera_up, 1.0f));

            auto camera_right = main_camera.getRightDirection();
            auto rotate_x = Rotate((float)(0.3f * delta_y * ecs::CameraComponent::Sensitivity), camera_right);
            main_camera.pos = rotate_x * Vec4(main_camera.pos, 1.0f);
            main_camera.direction = rotate_x * Vec4(main_camera.direction, 1.0f);
            main_camera.camera_up = Vec3(rotate_x * Vec4(main_camera.camera_up, 1.0f));

            main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, main_camera.camera_up);
        }
        else if (mouse_button == MouseButton::Right) {
            main_camera.pos += -(float)(delta_x * ecs::CameraComponent::Sensitivity) * main_camera.getRightDirection();
            main_camera.pos += -(float)(delta_y * ecs::CameraComponent::Sensitivity) * main_camera.getUpDirection();

            main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, main_camera.camera_up);
        }
    }

    if (main_camera.mode == ecs::CameraComponent::Mode::FPS) {
        // FPS style 自己不动，只转方向
        if (mouse_button == MouseButton::Left) {
            // get pitch
            main_camera.fps_params.pitch += delta_y * ecs::CameraComponent::Sensitivity;
            // get yaw
            main_camera.fps_params.yaw += delta_x * ecs::CameraComponent::Sensitivity;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            // 使用欧拉角导致的问题：在极点发生死锁，yaw和roll重合，失去了一个自由度
            if (main_camera.fps_params.pitch > 89.0f)
                main_camera.fps_params.pitch = 89.0f;
            if (main_camera.fps_params.pitch < -89.0f)
                main_camera.fps_params.pitch = -89.0f;

            // update direction
            main_camera.direction.x = cos(deg2rad(main_camera.fps_params.pitch)) * sin(deg2rad(main_camera.fps_params.yaw));
            main_camera.direction.y = sin(deg2rad(main_camera.fps_params.pitch));
            main_camera.direction.z = -cos(deg2rad(main_camera.fps_params.pitch)) * cos(deg2rad(main_camera.fps_params.yaw));
            
            main_camera.camera_up.x = sin(deg2rad(main_camera.fps_params.pitch)) * sin(deg2rad(main_camera.fps_params.yaw));
            main_camera.camera_up.y = cos(deg2rad(main_camera.fps_params.pitch));
            main_camera.camera_up.z = -sin(deg2rad(main_camera.fps_params.pitch)) * cos(deg2rad(main_camera.fps_params.yaw));

            main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, main_camera.camera_up);
        }
    }
}

void CameraSystem::orbitRotate(Vec3 start, Vec3 end)
{
    // 计算旋转角度角度
    float angle = acos(fmin(1.0f, Dot(start, end)));
    // 计算旋转轴
    Vec3 rotate_axis = Normalize(Cross(start, end));
    //Vec3 world_rotate_axis = Inverse(Mat3(main_camera.view)) * rotate_axis;
    
    Mat4 rotate_mat = Rotate(angle, rotate_axis);

    main_camera.pos = rotate_mat * Vec4(main_camera.pos, 1.0f);
    main_camera.direction = rotate_mat * Vec4(main_camera.direction, 1.0f);
    main_camera.camera_up = Vec3(rotate_mat * Vec4(main_camera.camera_up, 1.0f));
    main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, main_camera.camera_up);
}

void CameraSystem::onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y)
{
    if (main_camera.zoom_mode == ecs::CameraComponent::ZoomMode::ZoomToCenter) {
        main_camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
        if (main_camera.zoom < 0.1f)
            main_camera.zoom = 0.1f;

        m_goal_fov = main_camera.originFov / main_camera.zoom;
        if (m_goal_fov <= deg2rad(1.0f))
            m_goal_fov = deg2rad(1.0f);
        if (m_goal_fov >= deg2rad(135.0f))
            m_goal_fov = deg2rad(135.0f);

        m_need_update = true;
    }
    if (main_camera.zoom_mode == ecs::CameraComponent::ZoomMode::ZoomToMouse) {
        auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
        main_viewport.transToScreenCoordinates();
        mouse_x -= main_viewport.x;
        mouse_y -= main_viewport.y;
        Vec3 mouse_3d_pos = rayCastPlaneZero(mouse_x, mouse_y);

        Logger::get().debug("Mouse Ray");
        Logger::get().debug("Mouse 2d position: {},{}", mouse_x, mouse_y);
        Logger::get().debug("Mouse 3d position: {},{},{}", mouse_3d_pos.x, mouse_3d_pos.y, mouse_3d_pos.z);
        Logger::get().debug("\n");

        float viewport_width = (float)main_viewport.width;
        float viewport_height = (float)main_viewport.height;
        Vec3 center_3d_pos = rayCastPlaneZero(viewport_width / 2.0f, viewport_height / 2.0f);
        Vec3 displacement = mouse_3d_pos - center_3d_pos;

        if (yoffset == 0.0)
            return;
        // 1. first translate to mouse_3d_pos
        main_camera.pos += displacement;
        float old_zoom = main_camera.zoom;

        // 2. set zoom
        main_camera.zoom += ecs::CameraComponent::ZoomUnit * (float)yoffset;
        if (main_camera.zoom < 0.1f)
            main_camera.zoom = 0.1f;

        main_camera.fov = main_camera.originFov / main_camera.zoom;
        if (main_camera.fov <= deg2rad(1.0f))
            main_camera.fov = deg2rad(1.0f);
        if (main_camera.fov >= deg2rad(135.0f))
            main_camera.fov = deg2rad(135.0f);

        // 3. second translate back to original pos
        //main_camera.pos -= displacement * (old_zoom / main_camera.zoom);

        // 4. set view matrix, projection matrix
        main_camera.view = LookAt(main_camera.pos, main_camera.pos + main_camera.direction, main_camera.camera_up);
        main_camera.projection = Perspective(main_camera.fov, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    }
}

Vec3 CameraSystem::rayCastPlaneZero(double mouse_x, double mouse_y)
{

    // 1.  get ray direction from mouse position
    Vec3 cam_right = main_camera.getRightDirection();
    auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
    float viewport_width = (float)main_viewport.width;
    float viewport_height = (float)main_viewport.height;
    // normalized the x, y coordinate and take the viewport center as origin
    float u = 2.0f * mouse_x / viewport_width - 1.0f;
    float v = 2.0f * mouse_y / viewport_height - 1.0f;
    v = -v;

    float tangent = std::tan(main_camera.fov / 2.0f);
    Vec3 ray_direction = main_camera.direction + cam_right * tangent * u * (main_viewport.AspectRatio()) + main_camera.camera_up * tangent * v;
    ray_direction = Normalize(ray_direction);
    // 2.  solve the intersection equation of the ray and the plane: 
    // plane_normal. Dot(m_position + t * ray_direction - p0) = 0 
    //`Vec3 plane_normal = Vec3(0, 1, 0);
    Vec3 plane_normal = -main_camera.direction;
    Vec4 zero_plane = Vec4(plane_normal.x, plane_normal.y, plane_normal.z, 0);
    Vec3 p0 = plane_normal * zero_plane[3];
    float t = (Dot(plane_normal, p0) - Dot(plane_normal, main_camera.pos) / Dot(plane_normal, ray_direction));
    return Vec3(main_camera.pos + t * ray_direction);
}

}