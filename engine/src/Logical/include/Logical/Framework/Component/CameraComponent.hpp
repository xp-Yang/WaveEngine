#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "Logical/Framework/Component/Component.hpp"

enum class Mode {
	Orbit,
	FPS,
};

enum class Projection {
	Perspective,
	Ortho,
};

enum class ZoomMode {
	ZoomToCenter,
	ZoomToMouse,
};

struct CameraComponent : public Component {
	inline static Vec3 global_up = Vec3(0.0f, 1.0f, 0.0f); //vec3(0.0f, 1.0f, 0.0f) (yΪ��) or vec3(0.0f, 0.0f, 1.0f) (zΪ��)

	CameraComponent(GObject* parent) : Component(parent) {}
	CameraComponent(const CameraComponent&) = delete;
	CameraComponent& operator=(const CameraComponent&) = delete;

	Mode mode{ Mode::Orbit };

	Projection projection_mode{ Projection::Perspective };

	ZoomMode zoom_mode{ ZoomMode::ZoomToCenter };

	// FPS style
	// ŷ���Ǳ�ʾ��
	struct FPSParams {
		float pitch = 0.0f; // ������������������ϵ x-z ƽ��ļн�
		float yaw = 0.0f; // ������������������ϵ x-z ƽ���ͶӰʸ�������������ϵ -z ��ļн�
		float roll = 0.0f;
	} fps_params;

	float originFov = Math::deg2rad(45.0f); //��ֱfov

	float fov = originFov;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Math::Normalize(Vec3(0.0f, -0.6f, -0.8f));
	Vec3 upDirection = Math::Normalize(global_up - Math::Dot(global_up, direction) * direction); // camera �� y ��
	Vec3 getRightDirection() const { // camera �� x ��
		return Math::Cross(direction, upDirection);
	}
	Vec3 pos = Vec3(0.0f) - 115.0f * direction;
	Mat4 view = Math::LookAt(pos, pos + direction, global_up);
	float aspectRatio{ 16.0f / 9.0f }; // TODO should on window size change
	Mat4 projection = projection_mode == Projection::Perspective ?
		Math::Perspective(fov, aspectRatio, nearPlane, farPlane) :
		Math::Ortho(-15.0f * aspectRatio, 15.0f * aspectRatio, -15.0f, 15.0f, nearPlane, farPlane);
};

#endif // !CameraComponent_hpp
