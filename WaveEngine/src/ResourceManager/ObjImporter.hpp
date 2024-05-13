#ifndef ObjImporter_hpp
#define ObjImporter_hpp

#include "MeshData.hpp"
#include "Material.hpp"

class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;
namespace Assimp {
class Importer;
}

namespace Asset {

class ObjImporter {
public:
	ObjImporter() = default;
	~ObjImporter();
	bool load(const std::string& obj_file_path);
	std::shared_ptr<Asset::MeshData> meshDataOfNode(int ai_mesh_idx);
	Asset::Material materialOfNode(int ai_mesh_idx);
	std::vector<int> getSubMeshesIds() { return m_ai_meshes_ids; }

protected:
	std::vector<aiMesh*> collect_ai_meshes();
	std::shared_ptr<Asset::MeshData> load_sub_mesh_data(aiMesh* mesh);
	Asset::Material load_material(aiMaterial* material);

private:
	static std::unordered_map<std::string, Assimp::Importer*> m_importers;

	const aiScene* m_scene{ nullptr };
	std::vector<aiMesh*> m_ai_meshes;
	std::vector<int> m_ai_meshes_ids;

	std::string m_obj_filepath;
	std::string m_directory;
};

}

#endif
