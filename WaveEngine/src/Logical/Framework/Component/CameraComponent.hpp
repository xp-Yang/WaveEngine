#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "Component.hpp"
#include "Logical/Input/CameraManipulator.hpp"

struct CameraComponent : public Component {
	CameraComponent(GObject* parent) : Component(parent) {}
	CameraComponent(const CameraComponent&) = delete;
	CameraComponent& operator=(const CameraComponent&) = delete;

	std::string typeName() override { return "CameraComponent"; }

	CameraManipulator::Mode mode{ CameraManipulator::Mode::Orbit };

	CameraManipulator::Projection projection_mode{ CameraManipulator::Projection::Perspective };

	CameraManipulator::ZoomMode zoom_mode{ CameraManipulator::ZoomMode::ZoomToCenter };

	// FPS style
	// 欧拉角表示：
	struct FPSParams {
		float pitch = 0.0f; // 方向向量与世界坐标系 x-z 平面的夹角
		float yaw = 0.0f; // 方向向量在世界坐标系 x-z 平面的投影矢量相对世界坐标系 -z 轴的夹角
		float roll = 0.0f;
	} fps_params;

	float originFov = Math::deg2rad(45.0f); //竖直fov

	float fov = originFov;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Math::Normalize(Vec3(0.0f, -0.6f, -0.8f));
	Vec3 upDirection = Math::Normalize(CameraManipulator::global_up - Math::Dot(CameraManipulator::global_up, direction) * direction); // camera 的 y 轴
	Vec3 getRightDirection() const { // camera 的 x 轴
		return Math::Cross(direction, upDirection);
	}
	Vec3 pos = Vec3(0.0f) - 115.0f * direction;
	Mat4 view = Math::LookAt(pos, pos + direction, CameraManipulator::global_up);
	float aspectRatio{ 16.0f / 9.0f }; // TODO should on window size change
	Mat4 projection = projection_mode == CameraManipulator::Projection::Perspective ?
		Math::Perspective(fov, aspectRatio, nearPlane, farPlane) :
		Math::Ortho(-15.0f * aspectRatio, 15.0f * aspectRatio, -15.0f, 15.0f, nearPlane, farPlane);
};

#endif // !CameraComponent_hpp
