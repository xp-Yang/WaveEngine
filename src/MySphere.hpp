#ifndef MySphere_hpp
#define MySphere_hpp

#include "Object.hpp"

struct Triangle {
	glm::vec3 vertices[3];
};

class MySphere : public Object {
public:
	MySphere();
	void create_tetrahedron();
	void create_icosphere(int recursive_depth);
	std::vector<Triangle> subdivide(Triangle triangle);
	size_t get_vertices_count() { return m_triangles.size() * 3; }
	int get_recursive_depth() { return m_recursive_depth; }

private:
	std::vector<Triangle> m_triangles;
	glm::vec3 m_center;
	int m_recursive_depth = 0;
};

#endif