#ifndef Mesh_hpp
#define Mesh_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_uv;
};

struct Triangle {
	glm::vec3 vertices[3];
};

class Mesh {
public:
	Mesh() = default;
	Mesh(std::vector<Vertex> vertices);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void build();
	void reset();

	unsigned int get_VAO() const { return m_VAO; }
	size_t get_indices_count() const { return m_indices.size(); }
	size_t get_vertices_count() const { return m_vertices.size(); }

protected:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	void create_vbo();
	void create_vao();

public:
	static Mesh create_cube_mesh();
	static Mesh create_icosphere_mesh(int regression_depth);
	static Mesh create_quad_mesh();
};


#endif