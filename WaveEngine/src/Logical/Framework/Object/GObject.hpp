#ifndef GObject_hpp
#define GObject_hpp

#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/AnimationComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"
#include "Logical/Framework/Component/RigidComponent.hpp"

namespace Meta {
	namespace Register {
		void allMetaRegister();
	}
}

struct GObjectID {
	GObjectID() : id(++global_id) {}
	bool operator==(const GObjectID& rhs) {
		return id == rhs.id;
	}
	operator int() { return id; }

	int id;

	static inline int global_id = 0;
};

class GObject {
public:
	GObject(GObject* parent = nullptr) : m_id(GObjectID()) , m_parent(parent) { if (parent) parent->append(this); }
	void append(GObject* node) { m_children.push_back(node); }
	int index() const { return m_parent ? m_parent->indexOf(this) : -1; }
	int indexOf(const GObject* child) const;
	// TODO 这些方法如果自己是leaf就不对了
	void remove(const GObject* node);
	GObject* find(const GObject* node);
	const std::vector<GObject*> allLeaves2();
	const std::vector<GObject*> allLeaves();
	bool isLeaf() const { return m_children.empty(); }
	const std::vector<GObject*>& children() const { return m_children; }

	bool load();
	void save();

	GObjectID ID() const { return m_id; }

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
	GObject* m_parent;
	std::vector<GObject*> m_children;

private:
	friend void Meta::Register::allMetaRegister();

	GObjectID m_id;
	std::vector<std::shared_ptr<Component>> m_components;
};

#endif