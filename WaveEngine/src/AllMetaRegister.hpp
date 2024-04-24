#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/Meta.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
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
	registerSerializer<Vec2>(Serialization::readFromJson_Vec2, Serialization::writeToJson_Vec2);

	// Vec3
	registerClass<Vec3>();
	registerField(&Vec3::x, "x");
	registerField(&Vec3::y, "y");
	registerField(&Vec3::z, "z");
	registerSerializer<Vec3>(Serialization::readFromJson_Vec3, Serialization::writeToJson_Vec3);

	// Name
	registerClass<NameComponent>();
	registerField(&NameComponent::name, "name");
	registerSerializer<NameComponent>(Serialization::readFromJson_NameComponent, Serialization::writeToJson_NameComponent);

	// Transform
	registerClass<TransformComponent>();
	registerField(&TransformComponent::translation, "translation");
	registerField(&TransformComponent::rotation, "rotation");
	registerField(&TransformComponent::scale, "scale");
	registerMethod(&TransformComponent::transform, "transform");
	registerSerializer<TransformComponent>(Serialization::readFromJson_TransformComponent, Serialization::writeToJson_TransformComponent);
}

}

}

#endif