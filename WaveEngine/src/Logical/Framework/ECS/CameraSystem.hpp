#ifndef CameraSystem_hpp
#define CameraSystem_hpp

#include "Core/Math.hpp"
#include "Logical/Input/InputEnums.hpp"
#include "Components.hpp"

namespace ecs {

class CameraSystem{
public:
	CameraSystem();
	void onUpdate();
	void onKeyUpdate(int key, float frame_time);
	void onMouseUpdate(double delta_x, double delta_y, MouseButton mouse_button);
	void orbitRotate(Vec3 start, Vec3 end);
	void onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y);
	Vec3 rayCastPlaneZero(double mouse_x, double mouse_y);

protected:
	World& world;
	CameraComponent& main_camera;

	float m_goal_fov;

	bool m_need_update{ false };
};

}

#endif