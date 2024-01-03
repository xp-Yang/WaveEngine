#ifndef CameraSystem_hpp
#define CameraSystem_hpp

#include "Core/Vector.hpp"

namespace ecs {

struct CameraSystem{
	static void onKeyUpdate(int key, float frame_time);
	static void onMouseUpdate(double delta_x, double delta_y, int mouse_button);
	static void orbitRotate(Vec3 start, Vec3 end);
	static void onMouseWheelUpdate(double yoffset);
};

}

#endif