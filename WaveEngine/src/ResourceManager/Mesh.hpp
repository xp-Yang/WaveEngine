#ifndef Mesh_hpp
#define Mesh_hpp

#include <Core/Math.hpp>
#include "Material.hpp"

namespace Asset {

class MeshData {
public:
	static std::shared_ptr<MeshData> create_cube_mesh();
	static std::shared_ptr<MeshData> create_icosphere_mesh(int regression_depth);
	static std::shared_ptr<MeshData> create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v);
	static std::shared_ptr<MeshData> create_ground_mesh(const Vec2& size);
	static std::shared_ptr<MeshData> create_screen_mesh();

public:
	MeshData() = delete;
	MeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	//MeshData(const std::vector<Triangle>& triangles);
	~MeshData() { reset(); }

	void reset();

	const std::vector<unsigned int>& indices() const { return m_indices; }
	const std::vector<Vertex>& vertices() const { return m_vertices; }

protected:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
};

struct SubMesh {
	std::shared_ptr<MeshData> sub_mesh_data;
	std::shared_ptr<Material> material;
	Mat4 local_transform = Mat4(1.0f);
};

struct Mesh {
	std::vector<SubMesh> sub_meshes;
};

}

#endif