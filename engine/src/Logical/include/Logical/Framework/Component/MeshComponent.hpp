#ifndef MeshComponent_hpp
#define MeshComponent_hpp

#include "Logical/Framework/Component/Component.hpp"
#include "Logical/Mesh.hpp"

struct MeshComponent : public Component {
	MeshComponent(GObject* parent) : Component(parent) {}

	std::string typeName() override { return "MeshComponent"; }

	std::vector<std::shared_ptr<Mesh>> sub_meshes;
};

#endif // !MeshComponent_hpp
