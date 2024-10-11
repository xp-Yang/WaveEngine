//#ifndef ResourceImporter_hpp
//#define ResourceImporter_hpp
//
//// import files in formats other than WaveEngin Asset file
//
//#include "Core/Common.hpp"
//
//class aiNode;
//class aiScene;
//class aiMesh;
//class aiMaterial;
//namespace Assimp {
//	class Importer;
//}
//
//class Mesh;
//class Material;
//
//struct BoneInfo
//{
//	/*id is index in finalBoneMatrices*/
//	int id;
//	/*offset matrix transforms vertex from model space to bone space*/
//	Mat4 offset;
//};
//
//class ResourceImporter {
//public:
//	ResourceImporter() = default;
//	~ResourceImporter();
//	bool load(const std::string& obj_file_path);
//	std::shared_ptr<Mesh> meshDataOfNode(int ai_mesh_idx);
//	std::shared_ptr<Material> materialOfNode(int ai_mesh_idx);
//	const std::vector<int>& getSubMeshesIds() const { return m_ai_meshes_ids; }
//	const std::map<std::string, BoneInfo>& getBoneInfoMap() const { return m_BoneInfoMap; }
//	int getBoneCount() const { return m_BoneCounter; }
//
//protected:
//	std::vector<aiMesh*> collect_ai_meshes();
//	std::shared_ptr<Mesh> load_sub_mesh_data(aiMesh* mesh);
//	std::shared_ptr<Material> load_material(aiMaterial* material);
//	//void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
//
//private:
//	static std::unordered_map<std::string, Assimp::Importer*> m_importers;
//
//	const aiScene* m_scene{ nullptr };
//	std::vector<aiMesh*> m_ai_meshes;
//	std::vector<int> m_ai_meshes_ids;
//	std::map<std::string, BoneInfo> m_BoneInfoMap;
//	int m_BoneCounter;
//
//	std::string m_obj_filepath;
//	std::string m_directory;
//};
//
//#endif
