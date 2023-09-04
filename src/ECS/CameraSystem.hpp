#ifndef CameraSystem_hpp
#define CameraSystem_hpp

namespace ecs {

struct CameraSystem{
	static void key_process(int key, float frame_time);
	static void mouse_process(double delta_x, double delta_y, int mouse_button);
	static void mouse_scroll_process(double yoffset);
};

}

#endif