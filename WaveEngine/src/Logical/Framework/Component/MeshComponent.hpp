#ifndef MeshComponent_hpp
#define MeshComponent_hpp

#include "Component.hpp"
#include "ResourceManager/Mesh.hpp"
#include "ResourceManager/Material.hpp"

struct MeshComponent : public Component {
	MeshComponent(GObject* parent) : Component(parent) {}

	std::string typeName() override { return "MeshComponent"; }

	std::vector<Asset::SubMesh> sub_meshes;
};

#endif // !MeshComponent_hpp
