#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

class GameObject {
public:
	GameObject(const ecs::Entity& entity) : GameObject(nullptr, entity) {}
	GameObject(GameObject* parent, const ecs::Entity& entity) : m_parent(parent), m_entity(entity) { if (parent) parent->append(this); }
	void append(GameObject* node) { m_children.push_back(node); }
	// TODO 这些方法如果自己是leaf就不对了
	void remove(const ecs::Entity& entity) {
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
		for (auto child : m_children) {
			auto res = child->find(entity);
			if (res)
				return res;
		}
		return m_entity.getId() == entity.getId() ? this : nullptr;
	}
	const std::vector<GameObject*> allLeaves2() {
		if (m_children.empty())
			return { this };
		// 深度优先
		std::vector<GameObject*> leaves;
		for (auto child : m_children) {
			if (child->children().empty()) {
				leaves.push_back(child);
			}
			else {
				auto child_leaves = child->allLeaves2();
				leaves.insert(leaves.end(), child_leaves.begin(), child_leaves.end());
			}
		}
		return leaves;
	}
	const std::vector<GameObject*> allLeaves() {
		if (m_children.empty())
			return { this };
		// 广度优先
		std::vector<GameObject*> leaves;
		std::vector<GameObject*> nodes;
		nodes.push_back(this);
		while (!nodes.empty()) {
			auto node = nodes.back();
			nodes.pop_back();
			for (auto child : node->children()) {
				if (!child->children().empty()) {
					nodes.push_back(child);
				}
				else {
					leaves.push_back(child);
				}
			}
		}
		return leaves;
	}
	bool isLeaf() const { return m_children.empty(); }
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
	void removeSphere(size_t index = -1);
	int maxPointLightCount() const { return 256; }
	int pointLightCount() const { return m_point_light_count; }
	int sphereCount() const { return m_test_sphere_count; }
	GameObject* object(const ecs::Entity& entity) { return m_root_object->find(entity); }

protected:
	void init();
	void initMainCamera();
	void createSkybox();
	void createGround();
	void createDirectionalLight();

	void updateSpheresPosition();

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