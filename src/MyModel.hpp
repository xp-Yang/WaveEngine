#ifndef MyModel_hpp
#define MyModel_hpp
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Renderer.hpp"

struct SubModelData
{
	Mesh mesh;
	Material material;
};

class Model {
public:
	Model(const std::string& file_path);
	void load_model(const std::string& file_path);
	const std::vector<SubModelData>& get_datas() { return m_datas; }

private:
	std::vector<SubModelData> m_datas;
	void processNode(aiNode* node, const aiScene* scene);
	SubModelData load_mesh(aiMesh* mesh, const aiScene* scene);
	Material load_material(aiMaterial* material);
};

#endif
