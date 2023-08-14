#ifndef MyModel_hpp
#define MyModel_hpp
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MyMesh.hpp"
#include "Renderer.hpp"


class Model {
public:
	Model(const std::string& file_path);
	void load_model(const std::string& file_path);
	void draw(const Shader& shader);

private:
	std::vector<Mesh> m_meshes;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> load_textures(aiMaterial* material, aiTextureType type);

	Renderer m_renderer;
};

#endif
