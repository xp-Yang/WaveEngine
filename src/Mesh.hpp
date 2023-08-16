#ifndef Mesh_hpp
#define Mesh_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <vector>
#include <string>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_uv;
};

class Mesh {
public:
	Mesh() = default;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void build();

	unsigned int get_VAO() const { return m_VAO; }
	int get_indices_count() const { return m_indices.size(); }

	static unsigned int generate_texture_from_file(const std::string& full_path, bool gamma);
	static unsigned int generate_texture_from_file(const char* path, const std::string& directory, bool gamma);

protected:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	void create_vbo();
	void create_vao();
};


#endif