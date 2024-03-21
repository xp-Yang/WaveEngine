#ifndef CameraSystem_hpp
#define CameraSystem_hpp

#include "Core/Vector.hpp"
#include "GamePlay/Input/InputEnums.hpp"

namespace ecs {

struct CameraSystem{
	static void onKeyUpdate(int key, float frame_time);
	static void onMouseUpdate(double delta_x, double delta_y, MouseButton mouse_button);
	static void orbitRotate(Vec3 start, Vec3 end);
	static void onMouseWheelUpdate(double yoffset);
	static void onMouseWheelUpdate(double yoffset, double mouse_x, double mouse_y);
	static Vec3 rayCastPlaneZero(double mouse_x, double mouse_y);
};

}

#endif