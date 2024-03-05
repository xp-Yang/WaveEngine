#include "MotionSystem.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

void MotionSystem::onUpdate()
{
	auto& world = ecs::World::get();
	// light
	float avrg_frame_time = 1.0f / ImGui::GetIO().Framerate;
	// ÿһ֡����ʱ��Խ������ζ����һ֡����Ⱦ������Խ��ʱ�䣬������һ֡���ٶ�Ӧ��Խ����ƽ����Ⱦ����ȥ��ʱ��
	float delta = avrg_frame_time / 2.0f;
	Mat4 rotate = Rotate(delta, Vec3(0.0f, 1.0f, 0.0f));
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		auto& light_transform = *world.getComponent<ecs::TransformComponent>(entity);
		light_transform.translation = Vec3(rotate * Vec4(light_transform.translation, 1.0f));
	}
}
