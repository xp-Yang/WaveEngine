#ifndef MeshComponent_hpp
#define MeshComponent_hpp

#include "Component.hpp"
#include "ResourceManager/Mesh.hpp"
#include "ResourceManager/Material.hpp"

struct MeshComponent : public Component {
	MeshComponent(GObject* parent) : Component(parent) {}

	std::shared_ptr<Asset::Mesh> mesh;
};

#endif // !MeshComponent_hpp
