#ifndef Mesh_hpp
#define Mesh_hpp

#include <Core/Math/Math.hpp>
#include "Material.hpp"

namespace Asset {

enum class MeshFileType {
	None,
	OBJ,
	STL,
	JSON,
	CustomCube,
	CustomSphere,
	CustomGround,
	CustomGrid,
	CustomScreen,
};

struct MeshFileRef {
	MeshFileType mesh_file_type{ MeshFileType::None };
	std::string mesh_filepath;
};

struct SubMesh {
	int sub_mesh_idx{ 0 };
	MeshFileRef mesh_file_ref;
	Material material;
	Mat4 local_transform = Mat4(1.0f);
};

}

#endif