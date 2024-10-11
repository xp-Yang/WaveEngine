#ifndef MeshData_hpp
#define MeshData_hpp

#include <Core/Math/Math.hpp>

namespace Asset {

#define MAX_BONE_INFLUENCE 100
struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 texture_uv;

	std::vector<int> m_BoneIDs = std::vector<int>(MAX_BONE_INFLUENCE, -1);
	std::vector<float> m_Weights = std::vector<float>(MAX_BONE_INFLUENCE, 0.0f);
};

struct Triangle {
	Triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3) {
		vertices[0].position = pos1;
		vertices[1].position = pos2;
		vertices[2].position = pos3;
	}
	std::array<Vertex, 3> vertices;
};

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