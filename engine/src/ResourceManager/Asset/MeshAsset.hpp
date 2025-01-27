#ifndef MeshAsset_hpp
#define MeshAsset_hpp

#include <Core/Math/Math.hpp>

namespace Asset{

enum class MeshFileType {
	None,
	OBJ,
	STL,
	JSON,
	CustomCube,
	CustomSphere,
	CustomGround,
	CustomScreen,
};

struct MeshFileRef {
	MeshFileType mesh_file_type{ MeshFileType::None };
	std::string mesh_filepath;
};

struct MeshAssetDescriptor {
	int sub_mesh_idx{ 0 };
	MeshFileRef mesh_file_ref;
	//Material material;
	Mat4 local_transform = Mat4(1.0f);
};

}

#endif