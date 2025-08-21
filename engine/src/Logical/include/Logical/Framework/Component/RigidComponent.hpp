#ifndef RigidComponent_hpp
#define RigidComponent_hpp

#include "Logical/Framework/Component/Component.hpp"

struct RigidComponent : public Component {
	RigidComponent(GObject* parent) : Component(parent) {}
};

#endif // !RigidComponent_hpp
