#ifndef MyModel_hpp
#define MyModel_hpp
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MyMesh.hpp"
#include "MyRenderer.hpp"


class Model {
public:
	Model(const std::string& file_path);
	void load_model(const std::string& file_path);
	void draw(const MyShader& shader);

private:
	std::vector<Mesh> m_meshes;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	//std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
	//	std::string typeName);

	MyRenderer m_renderer;
};

#endif
