#include "MotionSystem.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

void MotionSystem::onUpdate()
{
	auto& world = ecs::World::get();
	// light
	float avrg_frame_time = 1.0f / ImGui::GetIO().Framerate;
	// 每一帧持续时间越长，意味着上一帧的渲染花费了越多时间，所以这一帧的速度应该越大，来平衡渲染所花去的时间
	float delta = avrg_frame_time / 2.0f;
	Mat4 rotate = Rotate(delta, Vec3(0.0f, 1.0f, 0.0f));
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		auto& light_transform = *world.getComponent<ecs::TransformComponent>(entity);
		light_transform.translation = Vec3(rotate * Vec4(light_transform.translation, 1.0f));
	}
}
