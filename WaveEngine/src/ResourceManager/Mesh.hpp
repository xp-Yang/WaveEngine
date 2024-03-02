#ifndef Mesh_hpp
#define Mesh_hpp
#include <Core/Vector.hpp>
#include <vector>
#include <string>

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 texture_uv;
};

struct Triangle {
	Vec3 vertices[3];
};

class Mesh {
public:
	Mesh() = default;
	Mesh(std::vector<Vertex> vertices);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	//~Mesh() { reset(); }

	void build();
	void reset();

	unsigned int get_VAO() const { return m_VAO; }
	size_t get_indices_count() const { return m_indices.size(); }
	size_t get_vertices_count() const { return m_vertices.size(); }

protected:
	unsigned int m_VAO = 0;
	unsigned int m_VBO = 0;
	unsigned int m_IBO = 0;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	void create_vbo();
	void create_vao();

public:
	static Mesh create_cube_mesh();
	static Mesh create_icosphere_mesh(int regression_depth);
	static Mesh create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v);
	static Mesh create_ground_mesh();
	static Mesh create_screen_mesh();
};


#endif