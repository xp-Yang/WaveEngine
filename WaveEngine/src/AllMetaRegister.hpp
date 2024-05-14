#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/Meta.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#include "Logical/Framework/ECS/SceneHierarchy.hpp"
#include "AllSerializer.hpp"

namespace Meta {

using namespace ecs;

namespace Register{

inline void allMetaRegister()
{
	// Vec2
	registerClass<Vec2>();
	registerField(&Vec2::x, "x");
	registerField(&Vec2::y, "y");

	// Vec3
	registerClass<Vec3>();
	registerField(&Vec3::x, "x");
	registerField(&Vec3::y, "y");
	registerField(&Vec3::z, "z");

	// Name
	registerClass<NameComponent>();
	registerField(&NameComponent::name, "name");

	// Transform
	registerClass<TransformComponent>();
	registerField(&TransformComponent::translation, "translation");
	registerField(&TransformComponent::rotation, "rotation");
	registerField(&TransformComponent::scale, "scale");
	registerMethod(&TransformComponent::transform, "transform");

	// Entity
	registerClass<Entity>();
	registerField(&Entity::m_id, "m_id");
	registerField(&Entity::m_mask, "m_mask");

	// ComponentPool
	registerClass<ComponentPool>();
	registerField(&ComponentPool::m_data, "m_data");
	registerField(&ComponentPool::m_componentTypeSize, "m_componentTypeSize");

	// World
	registerClass<World>();
	registerField(&World::m_entities, "m_entities");
	registerField(&World::m_component_pools, "m_component_pools");

}

}

}

#endif