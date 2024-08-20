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

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;
	/*offset matrix transforms vertex from model space to bone space*/
	Mat4 offset;
};

class ObjImporter {
public:
	ObjImporter() = default;
	~ObjImporter();
	bool load(const std::string& obj_file_path);
	std::shared_ptr<Asset::MeshData> meshDataOfNode(int ai_mesh_idx);
	Asset::Material materialOfNode(int ai_mesh_idx);
	const std::vector<int>& getSubMeshesIds() const { return m_ai_meshes_ids; }
	const std::map<std::string, BoneInfo>& getBoneInfoMap() const { return m_BoneInfoMap; }
	int getBoneCount() const { return m_BoneCounter; }

protected:
	std::vector<aiMesh*> collect_ai_meshes();
	std::shared_ptr<Asset::MeshData> load_sub_mesh_data(aiMesh* mesh);
	Asset::Material load_material(aiMaterial* material);
	void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh);

private:
	static std::unordered_map<std::string, Assimp::Importer*> m_importers;

	const aiScene* m_scene{ nullptr };
	std::vector<aiMesh*> m_ai_meshes;
	std::vector<int> m_ai_meshes_ids;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter;

	std::string m_obj_filepath;
	std::string m_directory;
};

}

#endif
