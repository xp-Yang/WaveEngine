#ifndef MyMesh_hpp
#define MyMesh_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/material.h>
#include <vector>
#include <string>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_uv;
};

struct Texture {
	unsigned int id;
	aiTextureType type;
	aiString path;
};

class Mesh {
public:
	Mesh() = default;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	unsigned int get_VAO() { return m_VAO; }
	int get_indices_count() { return m_indices.size(); }
	const std::vector<Texture>& get_texures() { return m_textures; }

protected:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;

	void create_vbo();
	void create_vao();
};


#endif