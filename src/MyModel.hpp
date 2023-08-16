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


class Model {
public:
	Model(const std::string& file_path);
	void load_model(const std::string& file_path);
	void draw(const Shader& shader, const Renderer& renderer);

private:
	std::vector<Mesh> m_meshes;
	std::vector<Material> m_materials;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh load_mesh(aiMesh* mesh, const aiScene* scene);
	Material load_material(aiMaterial* material);
};

#endif
