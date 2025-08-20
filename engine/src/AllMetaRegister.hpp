#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Meta.hpp"
#include "AllSerializer.hpp"

#include "Logical/Mesh.hpp"

#if ENABLE_ECS
#include "Logical/Framework/ECS/Components.hpp"
#endif

#include "Logical/Framework/World/Scene.hpp"
#include "Logical/Shader.hpp"

namespace Meta {

namespace Registration {

inline void allMetaRegister()
{
	// Vec2
	registerClass<Vec2>().
		registerProperty(&Vec2::x, "x").
		registerProperty(&Vec2::y, "y");

	// Vec3
	registerClass<Vec3>().
		registerProperty(&Vec3::x, "x").
		registerProperty(&Vec3::y, "y").
		registerProperty(&Vec3::z, "z");

	// Vec4
	registerClass<Vec4>().
		registerProperty(&Vec4::x, "x").
		registerProperty(&Vec4::y, "y").
		registerProperty(&Vec4::z, "z").
		registerProperty(&Vec4::w, "w");

	// Mat3
	registerClass<Mat3>();
	// TODO

	// Mat4
	registerClass<Mat4>();

	registerClass<Mesh>();
	registerClass<Material>();
	registerClass<Shader>();
	registerClass<Texture>();
	registerClass<CubeTexture>();

#if ENABLE_ECS
	// Name
	registerClass<ecs::NameComponent>();
	registerProperty(&ecs::NameComponent::name, "name");

	// Transform
	registerClass<ecs::TransformComponent>();
	registerProperty(&ecs::TransformComponent::translation, "translation");
	registerProperty(&ecs::TransformComponent::rotation, "rotation");
	registerProperty(&ecs::TransformComponent::scale, "scale");
	registerMethod(&ecs::TransformComponent::transform, "transform");

	// Entity
	registerClass<ecs::Entity>();
	registerProperty(&ecs::Entity::m_id, "m_id");
	//registerProperty(&ecs::Entity::m_mask, "m_mask");

	// ComponentPool
	registerClass<ecs::ComponentPool>();
	registerProperty(&ecs::ComponentPool::m_data, "m_data");
	registerProperty(&ecs::ComponentPool::m_componentTypeSize, "m_componentTypeSize", true);

	// World
	registerClass<ecs::World>();
	registerProperty(&ecs::World::m_entities, "m_entities", true);
	registerProperty(&ecs::World::m_component_pools, "m_component_pools", true);
#else
	registerClass<::GObjectID>().
		registerProperty(&::GObjectID::id, "id");

	registerClass<::Component>();

	registerClass<::GObject>().
		registerProperty(&::GObject::m_id, "m_id").
		registerProperty(&::GObject::m_components, "m_components");

	registerClass<::MeshComponent>().
		registerProperty(&::MeshComponent::sub_meshes, "sub_meshes");

	registerClass<::TransformComponent>().
		registerProperty(&::TransformComponent::translation, "translation").
		registerProperty(&::TransformComponent::rotation, "rotation").
		registerProperty(&::TransformComponent::scale, "scale");

	registerClass<::CameraComponent>().
		registerProperty(&::CameraComponent::mode, "mode").
		registerProperty(&::CameraComponent::projection_mode, "projection_mode").
		registerProperty(&::CameraComponent::zoom_mode, "zoom_mode").
		registerProperty(&::CameraComponent::originFov, "originFov").
		registerProperty(&::CameraComponent::fov, "fov").
		registerProperty(&::CameraComponent::nearPlane, "nearPlane").
		registerProperty(&::CameraComponent::farPlane, "farPlane").
		registerProperty(&::CameraComponent::direction, "direction").
		registerProperty(&::CameraComponent::pos, "pos").
		registerProperty(&::CameraComponent::upDirection, "upDirection").
		registerProperty(&::CameraComponent::view, "view").
		registerProperty(&::CameraComponent::projection, "projection");

	registerClass<::AnimationComponent>();
	registerClass<::RigidComponent>();
#endif

}

}

}

#endif