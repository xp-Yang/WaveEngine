#include "MotionSystem.hpp"
#include "Components.hpp"

namespace ecs {

void MotionSystem::onUpdate()
{
	auto& world = ecs::World::get();
	for (auto entity : world.entityView<ecs::TransformComponent>()) {
		auto& transform = *world.getComponent<ecs::TransformComponent>(entity);

	}
}

}