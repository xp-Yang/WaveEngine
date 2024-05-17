#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/Meta.hpp"
#include "AllSerializer.hpp"

#include "ResourceManager/AssetManager.hpp"

#if ENABLE_ECS
#include "Logical/Framework/ECS/Components.hpp"
#endif

#include "Logical/Framework/Scene.hpp"

namespace Meta {

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

	// Vec4
	registerClass<Vec4>();
	registerField(&Vec4::x, "x");
	registerField(&Vec4::y, "y");
	registerField(&Vec4::z, "z");
	registerField(&Vec4::w, "w");

	// Mat3
	registerClass<Mat3>();
	// TODO

	// Mat4
	registerClass<Mat4>();

	registerClass<Asset::MeshData>();
	registerClass<Asset::MeshFileRef>();
	registerClass<Asset::SubMesh>();
	registerClass<Asset::Material>();
	registerClass<Asset::Shader>();
	registerClass<Asset::Texture>();
	registerClass<Asset::CubeTexture>();

#if ENABLE_ECS
	// Name
	registerClass<ecs::NameComponent>();
	registerField(&ecs::NameComponent::name, "name");

	// Transform
	registerClass<ecs::TransformComponent>();
	registerField(&ecs::TransformComponent::translation, "translation");
	registerField(&ecs::TransformComponent::rotation, "rotation");
	registerField(&ecs::TransformComponent::scale, "scale");
	registerMethod(&ecs::TransformComponent::transform, "transform");

	// Entity
	registerClass<ecs::Entity>();
	registerField(&ecs::Entity::m_id, "m_id");
	//registerField(&ecs::Entity::m_mask, "m_mask");

	// ComponentPool
	registerClass<ecs::ComponentPool>();
	registerField(&ecs::ComponentPool::m_data, "m_data");
	registerField(&ecs::ComponentPool::m_componentTypeSize, "m_componentTypeSize", true);

	// World
	registerClass<ecs::World>();
	registerField(&ecs::World::m_entities, "m_entities", true);
	registerField(&ecs::World::m_component_pools, "m_component_pools", true);
#else
	registerClass<::GObjectID>();
	registerField(&::GObjectID::id, "id");

	registerClass<::Component>();

	registerClass<::GObject>();
	registerField(&::GObject::m_id, "m_id");
	registerField(&::GObject::m_components, "m_components", true);

	registerClass<::MeshComponent>();
	registerField(&::MeshComponent::sub_meshes, "sub_meshes", true);

	registerClass<::TransformComponent>();
	registerField(&::TransformComponent::translation, "translation");
	registerField(&::TransformComponent::rotation, "rotation");
	registerField(&::TransformComponent::scale, "scale");

	registerClass<::CameraComponent>();
	registerField(&::CameraComponent::mode, "mode");
	registerField(&::CameraComponent::projection_mode, "projection_mode");
	registerField(&::CameraComponent::zoom_mode, "zoom_mode");
	registerField(&::CameraComponent::zoom, "zoom");
	registerField(&::CameraComponent::originFov, "originFov");
	registerField(&::CameraComponent::fov, "fov");
	registerField(&::CameraComponent::nearPlane, "nearPlane");
	registerField(&::CameraComponent::farPlane, "farPlane");
	registerField(&::CameraComponent::direction, "direction");
	registerField(&::CameraComponent::pos, "pos");
	registerField(&::CameraComponent::upDirection, "upDirection");
	registerField(&::CameraComponent::view, "view");
	registerField(&::CameraComponent::projection, "projection");

	registerClass<::AnimationComponent>();
	registerClass<::RigidComponent>();
#endif

}

}

}

#endif