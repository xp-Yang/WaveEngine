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
	Vec3 upDirection = Math::Normalize(CameraManipulator::global_up - Math::Dot(CameraManipulator::global_up, direction) * direction); // camera �� y ��
	Vec3 getRightDirection() const { // camera �� x ��
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
