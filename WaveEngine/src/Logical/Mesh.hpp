#ifndef Mesh_hpp
#define Mesh_hpp

#include <Core/Math/Math.hpp>
#include "Material.hpp"

#define MAX_BONE_INFLUENCE 100
struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 texture_uv;

	//std::vector<int> m_BoneIDs = std::vector<int>(MAX_BONE_INFLUENCE, -1);
	//std::vector<float> m_Weights = std::vector<float>(MAX_BONE_INFLUENCE, 0.0f);
};

struct Triangle {
	Triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3) {
		vertices[0].position = pos1;
		vertices[1].position = pos2;
		vertices[2].position = pos3;
	}
	std::array<Vertex, 3> vertices;
};

struct Mesh {
	static std::shared_ptr<Mesh> create_cube_mesh();
	static std::shared_ptr<Mesh> create_cuboid_mesh(const std::array<Vec3, 8> vertex_positions);
	static std::shared_ptr<Mesh> create_icosphere_mesh(float radius, int regression_depth);
	static std::shared_ptr<Mesh> create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v);
	static std::shared_ptr<Mesh> create_complex_quad_mesh(const Vec2& size);
	static std::shared_ptr<Mesh> create_screen_mesh();

	Mesh() = delete;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, std::shared_ptr<Material> material_);
	//Mesh(const std::vector<Triangle>& triangles);

	void reset();

	int sub_mesh_idx{ 0 };
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	std::shared_ptr<Material> material;
	Mat4 local_transform = Mat4(1.0f);
};

#endif