#ifndef SimpleMesh_hpp
#define SimpleMesh_hpp

#include <Core/Math/Math.hpp>

struct SimpleVertex {
	Vec3 position;
	Vec3 normal;
};

struct SimpleMesh {
	SimpleMesh(const std::vector<SimpleVertex>& vertices, const std::vector<int>& indices) : vertices(vertices), indices(indices) {}

	void reset() {
		vertices.clear();
		indices.clear();
		vertices.shrink_to_fit();
		indices.shrink_to_fit();
	}

	std::vector<SimpleVertex> vertices;
	std::vector<int> indices;

	static std::shared_ptr<SimpleMesh> create_vertex_normal_cuboid_mesh(const std::array<Vec3, 8> vertex_positions);
	static std::shared_ptr<SimpleMesh> merge(const std::vector<std::shared_ptr<SimpleMesh>>& meshes);
};

#endif