#include "MotionSystem.hpp"
#include "Components.hpp"
#include <GLFW/glfw3.h>

namespace ecs {

void MotionSystem::onUpdate()
{
	auto& world = ecs::World::get();
	// light
	float delta = 0.01f;
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), delta, glm::vec3(0.0f, 1.0f, 0.0f));
	for (auto entity : world.entityView<ecs::LightComponent, ecs::TransformComponent>()) {
		auto& light_transform = *world.getComponent<ecs::TransformComponent>(entity);
		light_transform.translation = glm::vec3(rotate * glm::vec4(light_transform.translation, 1.0f));
	}
}

}