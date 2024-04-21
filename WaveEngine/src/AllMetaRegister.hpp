#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/Meta.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

namespace Meta {
namespace Register{

inline void allMetaRegister()
{
	// Name
	registerClass<ecs::NameComponent>();
	registerField(&ecs::NameComponent::name, "name");

	// Transform
	registerClass<ecs::TransformComponent>();
	registerField(&ecs::TransformComponent::translation, "translation");
	registerField(&ecs::TransformComponent::rotation, "rotation");
	registerField(&ecs::TransformComponent::scale, "scale");
	registerMethod(&ecs::TransformComponent::transform, "transform");
}

}
}

#endif