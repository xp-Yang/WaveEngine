#include "SimpleMesh.hpp"

std::shared_ptr<SimpleMesh> SimpleMesh::create_vertex_normal_cuboid_mesh(const std::array<Vec3, 8> vertex_positions)
{
    std::vector<SimpleVertex> vertices(8);

    Vec3 front_face_center = (vertex_positions[0] + vertex_positions[2]) / 2.0f;
    Vec3 back_face_center = (vertex_positions[4] + vertex_positions[6]) / 2.0f;
    std::array<Vec3, 8> vertex_normals = {
        Math::Normalize(vertex_positions[0] - front_face_center),
        Math::Normalize(vertex_positions[1] - front_face_center),
        Math::Normalize(vertex_positions[2] - front_face_center),
        Math::Normalize(vertex_positions[3] - front_face_center),
        Math::Normalize(vertex_positions[4] - back_face_center),
        Math::Normalize(vertex_positions[5] - back_face_center),
        Math::Normalize(vertex_positions[6] - back_face_center),
        Math::Normalize(vertex_positions[7] - back_face_center),
    };

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] = { vertex_positions[i], vertex_normals[i] };
    }

    std::vector<int> indices = {
        0, 1, 2, 0, 2, 3,//前
        4, 5, 6, 4, 6, 7,//后
        7, 6, 1, 7, 1, 0,//左
        3, 2, 5, 3, 5, 4,//右
        1, 6, 5, 1, 5, 2,//下
        7, 0, 3, 7, 3, 4,//上
    };

    return std::make_shared<SimpleMesh>(vertices, indices);
}

std::shared_ptr<SimpleMesh> SimpleMesh::merge(const std::vector<std::shared_ptr<SimpleMesh>>& meshes)
{
    std::vector<SimpleVertex> vertices;
    std::vector<int> indices;
    vertices.reserve(8 * meshes.size());
    indices.reserve(36 * meshes.size());


    for (auto& mesh : meshes) {
        vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
    }

    int indices_offset = 0;
    for (int i = 0; i < meshes.size(); i++) {
        std::vector<int> indices_i(meshes[i]->indices.size());
        std::transform(meshes[i]->indices.begin(), meshes[i]->indices.end(), indices_i.begin(), [indices_offset](const auto& index) {
            return index + indices_offset;
            });
        indices.insert(indices.end(), indices_i.begin(), indices_i.end());
        indices_offset += meshes[i]->vertices.size();
    }

    return std::make_shared<SimpleMesh>(vertices, indices);
}