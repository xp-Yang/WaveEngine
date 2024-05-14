#ifndef Object_hpp
#define Object_hpp

#include "Components.hpp"

namespace ecs {

class Object {
public:
	Object(const Entity& entity) : Object(nullptr, entity) {}
	Object(Object* parent, const Entity& entity) : m_parent(parent), m_entity(entity) { if (parent) parent->append(this); }
	void append(Object* node) { m_children.push_back(node); }
	int index() const {
		return m_parent ? m_parent->indexOf(this) : -1;
	}
	int indexOf(const Object* child) const {
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
	void remove(const Entity& entity) {
		for (auto child : m_children) {
			child->remove(entity);
		}
		auto it = std::find_if(m_children.begin(), m_children.end(), [entity](Object* child) {
			return (*child).entity().getId() == entity.getId();
			});
		if (it != m_children.end()) {
			World::get().removeComponent<AllComponents>(entity);
			m_children.erase(it);
		}
	}
	void remove(Object* node) { remove(node->entity()); }
	Object* find(const Entity& entity) {
		for (auto child : m_children) {
			auto res = child->find(entity);
			if (res)
				return res;
		}
		return m_entity.getId() == entity.getId() ? this : nullptr;
	}
	const std::vector<Object*> allLeaves2() {
		if (isLeaf())
			return { this };
		// 深度优先
		std::vector<Object*> leaves;
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
	const std::vector<Object*> allLeaves() {
		if (isLeaf())
			return { this };
		// 广度优先
		std::vector<Object*> leaves;
		std::vector<Object*> nodes;
		nodes.push_back(this);
		while (!nodes.empty()) {
			auto node = nodes.front();
			nodes.erase(nodes.begin());
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
	const std::vector<Object*>& children() const { return m_children; }
	const Entity& entity() const { return m_entity; }

private:
	Object* m_parent;
	std::vector<Object*> m_children;
	const Entity m_entity;
};

}

#endif