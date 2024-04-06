#ifndef Mesh_hpp
#define Mesh_hpp
#include <Core/Math.hpp>
#include <vector>
#include <string>

class Mesh {
public:
	static Mesh create_cube_mesh();
	static Mesh create_icosphere_mesh(int regression_depth);
	static Mesh create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v);
	static Mesh create_ground_mesh(const Vec2& size);
	static Mesh create_screen_mesh();

public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	//Mesh(const std::vector<Triangle>& triangles);

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
};


#endif