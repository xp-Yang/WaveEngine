#ifndef RigidComponent_hpp
#define RigidComponent_hpp

#include "Component.hpp"

struct RigidComponent : public Component {
	RigidComponent(GObject* parent) : Component(parent) {}

	std::string typeName() override { return "RigidComponent"; }
};

#endif // !RigidComponent_hpp
