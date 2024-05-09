#ifndef ObjImporter_hpp
#define ObjImporter_hpp
#include <string>
#include <vector>
#include "Mesh.hpp"
#include "Material.hpp"

class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;

namespace Asset {

class ObjImporter {
public:
	static ObjImporter& get() {
		static ObjImporter instance;
		return instance;
	}

	std::shared_ptr<Asset::Mesh> load(const std::string& obj_file_path);

private:
	ObjImporter() = default;

	void processNode(aiNode* node, const aiScene* scene);
	std::shared_ptr<Asset::MeshData> load_sub_mesh_data(aiMesh* mesh, const aiScene* scene);
	std::shared_ptr<Asset::Material> load_material(aiMaterial* material);

	std::shared_ptr<Asset::Mesh> m_data;
	std::string m_directory;
};

}

#endif
