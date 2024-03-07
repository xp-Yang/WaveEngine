#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "GamePlay/Framework/ECS/World.hpp"

class GameObject {
public:
	GameObject(GameObject* parent, const ecs::Entity& entity) : m_parent(parent), m_entity(entity) {
		if (parent)
			parent->append(this);
	}
	void append(GameObject* node) { m_children.push_back(node); }
	void remove(GameObject* node) {
		auto it = std::find_if(m_children.begin(), m_children.end(), [node](GameObject* child) {
			return (*child).entity().getId() == (*node).entity().getId();
			});
		if (it != m_children.end())
			m_children.erase(it);
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
	const std::vector<GameObject*>& selectedObjects() const { return selected_objects; }
	GameObject* selectedObject() const { return selected_objects.size() == 1 ? selected_objects[0] : nullptr; }
	void addObject(GameObject* obj);
	void removeObject(GameObject* obj);
	//Object* object(const std::string& name) {
	//	auto it = m_objects.find(name);
	//	if (it == m_objects.end())
	//		return nullptr;
	//	else
	//		return m_objects[name];
	//}
	//Object* object(const std::string& name) const {
	//	auto it = m_objects.find(name);
	//	if (it == m_objects.end())
	//		return nullptr;
	//	else
	//		return m_objects.at(name);
	//}
	//const std::unordered_map<std::string, Object*>& get_objects() const { return m_objects; }
	//bool insert_object(const std::string& name, Object* object) { return m_objects.insert({ name, object }).second; }
	//void remove_object(const std::string& name) { m_objects.erase(name); };

protected:
	void init();

private:
	GameObject* m_root_object{ nullptr };
	std::vector<GameObject*> selected_objects;

};

#endif