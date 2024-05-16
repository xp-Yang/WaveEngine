#ifndef CameraManipulator_hpp
#define CameraManipulator_hpp

#include "Core/Math.hpp"
#include "Logical/Input/InputEnums.hpp"

#include "Logical/Framework/ECS/Components.hpp"

class CameraManipulator{
public:
	CameraManipulator();
	void onUpdate();
	void onKeyUpdate(int key, float frame_time);
	void onMouseUpdate(double delta_x, double delta_y, MouseButton mouse_button);
	void orbitRotate(Vec3 start, Vec3 end);
	void onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y);
	Vec3 rayCastPlaneZero(double mouse_x, double mouse_y);

	inline static const float CameraMovementSpeed = 20.0f;
	inline static const float Sensitivity = 0.01f;
	inline static const float ZoomUnit = 0.1f;

protected:
	ecs::World& world;
	ecs::CameraComponent& main_camera;

	float m_goal_fov;

	bool m_need_update{ false };
};

#endif