#include "MotionSystem.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>

namespace ecs {

void MotionSystem::onUpdate()
{
	auto& world = ecs::World::get();
	// light
	static float time_value = 0.0f;
	time_value = (float)glfwGetTime();
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value, glm::vec3(0.0f, 1.0f, 0.0f));
	for (auto entity : world.entityView<ecs::LightComponent, ecs::TransformComponent>()) {
		auto& light_transform = *world.getComponent<ecs::TransformComponent>(entity);
		static auto original_translate = glm::vec4(light_transform.translation, 1.0f);
		light_transform.translation = rotate * original_translate;
	}
}

}