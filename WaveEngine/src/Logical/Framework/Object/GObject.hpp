#ifndef GObject_hpp
#define GObject_hpp

#include "Logical/Framework/World/IDAllocator.hpp"
#if !ENABLE_ECS
#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/AnimationComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"
#include "Logical/Framework/Component/RigidComponent.hpp"
#else
#include "Logical/Framework/ECS/Components.hpp"
#endif

namespace Meta {
	namespace Register {
		void allMetaRegister();
	}
}

#if !ENABLE_ECS
class GObjectID : public IDAllocator<GObjectID> {
public:
	GObjectID() = default;
};

class GObject {
public:
	static GObject* create(GObject* parent, const std::string& name);
	~GObject();
	void append(GObject* node) { m_children.push_back(node); }
	int index() const { return m_parent ? m_parent->indexOf(this) : -1; }
	int indexOf(const GObject* child) const;
	bool remove(GObject* node = nullptr);
	bool include(const GObject* node);
	const std::vector<GObject*> allLeaves2();
	const std::vector<GObject*> allLeaves();
	bool isLeaf() const { return m_children.empty(); }
	const std::vector<GObject*>& children() const { return m_children; }

	bool load();
	void save();

	GObjectID ID() const { return m_id; }
	const std::string& name() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }
	bool visible() const { return m_visible; }
	void setVisible(bool visible) { m_visible = visible; }

	const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

	template<typename T>
	T& addComponent() {
		auto com = std::make_shared<T>(this);
		m_components.push_back(com);
		return *com;
	}

	template<typename T>
	bool hasComponent() const {
		for (auto& component : m_components)
		{
			if (component->typeName() == Meta::traits::className<T>())
				return true;
		}
		return false;
	}

	template<typename T>
	T* getComponent()
	{
		for (auto& component : m_components)
		{
			if (component->typeName() == Meta::traits::className<T>())
				return static_cast<T*>(component.get());
		}
		return nullptr;
	}

	virtual void tick(float delta_time);

protected:
	GObject(GObject* parent, const std::string& name) : m_parent(parent), m_name(name) { if (parent) parent->append(this); }
	GObject(const GObject&) = delete;
	GObject& operator=(const GObject&) = delete;

	friend void Meta::Register::allMetaRegister();

	GObject* m_parent;
	std::vector<GObject*> m_children;
	GObjectID m_id;
	std::string m_name;
	bool m_visible{ true };
	std::vector<std::shared_ptr<Component>> m_components;
};

#else
class GObject {
public:
	GObject(const ecs::Entity& entity) : GObject(nullptr, entity) {}
	GObject(GObject* parent, const ecs::Entity& entity) : m_parent(parent), m_entity(entity) { if (parent) parent->append(this); }
	void append(GObject* node) { m_children.push_back(node); }
	int index() const {
		return m_parent ? m_parent->indexOf(this) : -1;
	}
	int indexOf(const GObject* child) const {
		if (!child)
			return -1;
		for (int i = 0; i < m_children.size(); i++) {
			if (m_children[i]->entity() == child->entity()) {
				return i;
			}
		}
		return -1;
	}
	void remove(const ecs::Entity& entity) {
		for (auto child : m_children) {
			child->remove(entity);
		}
		auto it = std::find_if(m_children.begin(), m_children.end(), [entity](GObject* child) {
			return (*child).entity().getId() == entity.getId();
			});
		if (it != m_children.end()) {
			ecs::World::get().removeComponent<AllComponents>(entity);
			m_children.erase(it);
		}
	}
	void remove(GObject* node) { remove(node->entity()); }
	GObject* find(const ecs::Entity& entity) {
		for (auto child : m_children) {
			auto res = child->find(entity);
			if (res)
				return res;
		}
		return m_entity.getId() == entity.getId() ? this : nullptr;
	}
	const std::vector<GObject*> allLeaves2() {
		if (isLeaf())
			return { this };
		// �������
		std::vector<GObject*> leaves;
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
	const std::vector<GObject*> allLeaves() {
		if (isLeaf())
			return { this };
		// �������
		std::vector<GObject*> leaves;
		std::vector<GObject*> nodes;
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
	const std::vector<GObject*>& children() const { return m_children; }
	const ecs::Entity& entity() const { return m_entity; }

private:
	GObject* m_parent;
	std::vector<GObject*> m_children;
	const ecs::Entity m_entity;
};
#endif // !

#endif