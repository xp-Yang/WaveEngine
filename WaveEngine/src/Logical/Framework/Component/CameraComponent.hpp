#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "Component.hpp"
#include <WaveEngine/Window.hpp>

struct CameraComponent {
	enum Mode {
		Orbit,
		FPS,
	} mode{ Orbit };

	enum Projection {
		Perspective,
		Ortho,
	} projection_mode{ Perspective };

	enum ZoomMode {
		ZoomToCenter,
		ZoomToMouse,
	} zoom_mode{ ZoomToCenter };

	// FPS style
	// 欧拉角表示：
	struct FPSParams {
		float pitch = 0.0f; // 方向向量与世界坐标系 x-z 平面的夹角
		float yaw = 0.0f; // 方向向量在世界坐标系 x-z 平面的投影矢量相对世界坐标系 -z 轴的夹角
		float roll = 0.0f;
	} fps_params;

	static const float CameraMovementSpeed;
	static const float Sensitivity;
	static const float ZoomUnit;
	static Vec3 global_up; //vec3(0.0f, 1.0f, 0.0f) (y为上) or vec3(0.0f, 0.0f, 1.0f) (z为上)

	float originFov = deg2rad(45.0f); //竖直fov
	float zoom = 1.0f;
	float fov = originFov / zoom;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f, -1.0f));
	Vec3 pos = Vec3(0.0f) - 40.0f * direction;
	Vec3 camera_up = Normalize(global_up - Dot(global_up, direction) * direction);
	Mat4 view = lookAt(pos, pos + direction, global_up);
	Mat4 projection = projection_mode == Perspective ? Math::Perspective(fov, ASPECT_RATIO, nearPlane, farPlane)
		: Math::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, nearPlane, farPlane);

	Vec3 getRightDirection() const { // camera 的 x 轴
		return Cross(direction, camera_up);
		Vec3 cameraRight = Normalize(Cross(direction, global_up));
		return cameraRight;
	}

	Vec3 getUpDirection() const { // camera 的 y 轴
		return camera_up;
	}
};

#endif // !CameraComponent_hpp
