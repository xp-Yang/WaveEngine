#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

class GameObject {
public:
	GameObject(const ecs::Entity& entity) : GameObject(nullptr, entity) {}
	GameObject(GameObject* parent, const ecs::Entity& entity) : m_parent(parent), m_entity(entity) { if (parent) parent->append(this); }
	void append(GameObject* node) { m_children.push_back(node); }
	void remove(const ecs::Entity& entity) {
		// TODO 改深度递归为循环
		for (auto child : m_children) {
			child->remove(entity);
		}
		auto it = std::find_if(m_children.begin(), m_children.end(), [entity](GameObject* child) {
			return (*child).entity().getId() == entity.getId();
			});
		if (it != m_children.end()) {
			ecs::World::get().removeComponent<AllComponents>(entity);
			m_children.erase(it);
		}
	}
	void remove(GameObject* node) { remove(node->entity()); }
	GameObject* find(const ecs::Entity& entity) {
		// TODO 改深度递归为循环
		for (auto child : m_children) {
			auto res = child->find(entity);
			if (res)
				return res;
		}
		return m_entity.getId() == entity.getId() ? this : nullptr;
	}
	const std::vector<GameObject*>& children() const { return m_children; }
	const ecs::Entity& entity() const { return m_entity; }

private:
	GameObject* m_parent;
	std::vector<GameObject*> m_children;
	const ecs::Entity m_entity; // TODO 初始化，默认构造被delete了不报错？
};

class SceneHierarchy {
public:
	SceneHierarchy();
	GameObject* rootObject() const { return m_root_object; }
	GameObject* loadModal(const std::string& filepath);
	//const std::vector<GameObject*>& selectedObjects() const { return selected_objects; }
	//GameObject* selectedObject() const { return selected_objects.size() == 1 ? selected_objects[0] : nullptr; }
	void addObject(GameObject* obj, GameObject* parent = nullptr) { 
		if (ecs::World::get().hasComponent<ecs::PointLightComponent>(obj->entity()))
			m_point_light_count++;
		parent ? parent->append(obj) : m_root_object->append(obj); 
	}
	void removeObject(const ecs::Entity& entity) {
		if (ecs::World::get().hasComponent<ecs::PointLightComponent>(entity))
			m_point_light_count--;
		m_root_object->remove(entity);
	}
	void removeObject(GameObject* obj) { 
		removeObject(obj->entity());
	}
	void addPointLight();
	void addCube();
	void addSphere();
	int maxPointLightCount() const { return 256; }
	int pointLightCount() const { return m_point_light_count; }
	GameObject* object(const ecs::Entity& entity) { return m_root_object->find(entity); }

protected:
	void init();
	void initMainCamera();
	void createSkybox();
	void createDirectionalLight();

private:
	GameObject* m_root_object{ nullptr };
	GameObject* m_root_point_light_object{ nullptr };
	GameObject* m_root_cube_object{ nullptr };
	GameObject* m_root_sphere_object{ nullptr };
	int m_point_light_count = 0;
	int m_test_cube_count = 0;
	int m_test_sphere_count = 0;
};

#endif