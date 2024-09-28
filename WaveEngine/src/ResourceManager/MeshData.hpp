#ifndef MeshData_hpp
#define MeshData_hpp

#include <Core/Math/Math.hpp>

namespace Asset {

class MeshData {
public:
	static std::shared_ptr<MeshData> create_cube_mesh();
	static std::shared_ptr<MeshData> create_icosphere_mesh(float radius, int regression_depth);
	static std::shared_ptr<MeshData> create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v);
	static std::shared_ptr<MeshData> create_complex_quad_mesh(const Vec2& size);
	static std::shared_ptr<MeshData> create_screen_mesh();

public:
	MeshData() = delete;
	MeshData(const std::vector<Vertex>& vertices, const std::vector<int>& indices);
	//MeshData(const std::vector<Triangle>& triangles);
	~MeshData() { reset(); }

	void reset();

	const std::vector<int>& indices() const { return m_indices; }
	const std::vector<Vertex>& vertices() const { return m_vertices; }

protected:
	std::vector<Vertex> m_vertices;
	std::vector<int> m_indices;
};

}

#endif