#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "Component.hpp"
#include <WaveEngine/Window.hpp>

struct CameraComponent : public Component {
	CameraComponent(GObject* parent) : Component(parent) {}

	std::string typeName() override { return "CameraComponent"; }

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
	// ŷ���Ǳ�ʾ��
	struct FPSParams {
		float pitch = 0.0f; // ������������������ϵ x-z ƽ��ļн�
		float yaw = 0.0f; // ������������������ϵ x-z ƽ���ͶӰʸ�������������ϵ -z ��ļн�
		float roll = 0.0f;
	} fps_params;

	inline static Vec3 CameraComponent::global_up = Vec3(0.0f, 1.0f, 0.0f); //vec3(0.0f, 1.0f, 0.0f) (yΪ��) or vec3(0.0f, 0.0f, 1.0f) (zΪ��)

	float originFov = deg2rad(45.0f); //��ֱfov

	float zoom = 1.0f;
	float fov = originFov / zoom;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f, -1.0f));
	Vec3 upDirection = Normalize(global_up - Dot(global_up, direction) * direction); // camera �� y ��
	Vec3 getRightDirection() const { // camera �� x ��
		return Cross(direction, upDirection);
	}
	Vec3 pos = Vec3(0.0f) - 40.0f * direction;
	Mat4 view = lookAt(pos, pos + direction, global_up);
	Mat4 projection = projection_mode == Perspective ?
		Math::Perspective(fov, ASPECT_RATIO, nearPlane, farPlane) :
		Math::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, nearPlane, farPlane);
};

#endif // !CameraComponent_hpp
