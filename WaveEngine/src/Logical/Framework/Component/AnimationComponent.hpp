#ifndef AnimationComponent_hpp
#define AnimationComponent_hpp

#include "Component.hpp"

struct AnimationComponent : public Component {
	AnimationComponent(GObject* parent) : Component(parent) {}

	std::string typeName() override { return "AnimationComponent"; }

};

#endif // !AnimationComponent_hpp
