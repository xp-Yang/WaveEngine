#ifndef GameObject_hpp
#define GameObject_hpp

#include "GamePlay/Framework/ECS/Components.hpp"

class GameObject {
public:
	GameObject(const ecs::Entity& entity) : GameObject(nullptr, entity) {}
	GameObject(GameObject* parent, const ecs::Entity& entity) : m_parent(parent), m_entity(entity) { if (parent) parent->append(this); }
	void append(GameObject* node) { m_children.push_back(node); }
	int index() const {
		return m_parent ? m_parent->indexOf(this) : -1;
	}
	int indexOf(const GameObject* child) const {
		if (!child)
			return -1;
		for (int i = 0; i < m_children.size(); i++) {
			if (m_children[i]->entity() == child->entity()) {
				return i;
			}
		}
		return -1;
	}
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
		if (isLeaf())
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
		if (isLeaf())
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

#endif