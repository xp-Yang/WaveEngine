#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "Component.hpp"
#include "Logical/Input/CameraManipulator.hpp"

// TODO
extern float WINDOW_WIDTH;
extern float WINDOW_HEIGHT;
extern float ASPECT_RATIO;

struct CameraComponent : public Component {
	CameraComponent(GObject* parent) : Component(parent) {}

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

	float originFov = deg2rad(45.0f); //��ֱfov

	float zoom = 1.0f;
	float fov = originFov / zoom;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f, -1.0f));
	Vec3 upDirection = Normalize(CameraManipulator::global_up - Dot(CameraManipulator::global_up, direction) * direction); // camera �� y ��
	Vec3 getRightDirection() const { // camera �� x ��
		return Cross(direction, upDirection);
	}
	Vec3 pos = Vec3(0.0f) - 40.0f * direction;
	Mat4 view = lookAt(pos, pos + direction, CameraManipulator::global_up);
	Mat4 projection = projection_mode == CameraManipulator::Projection::Perspective ?
		Math::Perspective(fov, ASPECT_RATIO, nearPlane, farPlane) :
		Math::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, nearPlane, farPlane);
};

#endif // !CameraComponent_hpp
