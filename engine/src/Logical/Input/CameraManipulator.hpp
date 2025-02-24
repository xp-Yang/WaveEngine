#ifndef CameraManipulator_hpp
#define CameraManipulator_hpp

#include "Base/Math/Math.hpp"
#include "Logical/Input/InputEnums.hpp"
#include "GUI/Viewport.hpp"

#if ENABLE_ECS
namespace ecs {
	struct CameraComponent;
}
#else
struct CameraComponent;
#endif

class CameraManipulator{
public:
	CameraManipulator(CameraComponent& camera);
	void syncContext(const Viewport& viewport);
	void onUpdate();
	void onKeyUpdate(int key, float frame_time);
	void onMouseUpdate(double delta_x, double delta_y, MouseButton mouse_button);
	void orbitRotate(Vec3 start, Vec3 end);
	void onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y);
	Vec3 rayCastPlaneZero(double mouse_x, double mouse_y);

	inline static const float CameraMovementSpeed = 20.0f;
	inline static const float RatationSensitivity = 0.01f;
	inline static const float PanSensitivity = 0.135f;
	inline static const float ZoomUnit = 0.1f;

protected:
#if ENABLE_ECS
	ecs::CameraComponent& main_camera;
#else 
	CameraComponent& main_camera;
#endif

	Viewport m_viewport;

	float m_goal_fov;

	bool m_need_update{ false };
};

#endif