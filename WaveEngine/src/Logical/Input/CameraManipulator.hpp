#ifndef CameraManipulator_hpp
#define CameraManipulator_hpp

#include "Core/Math/Math.hpp"
#include "Logical/Input/InputEnums.hpp"
#include "Render/RHI/Viewport.hpp"

#if ENABLE_ECS
namespace ecs {
	struct CameraComponent;
}
#else
struct CameraComponent;
#endif

class CameraManipulator{
public:
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

	inline static Vec3 global_up = Vec3(0.0f, 1.0f, 0.0f); //vec3(0.0f, 1.0f, 0.0f) (y为上) or vec3(0.0f, 0.0f, 1.0f) (z为上)

	CameraManipulator(CameraComponent& camera);
	void syncContext(const Viewport& viewport);
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