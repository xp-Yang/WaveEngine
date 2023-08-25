#include "MySphere.hpp"

MySphere::MySphere() {
    create_icosphere(4);
}

void MySphere::create_tetrahedron() {
    float side_length = 5.0f;
    glm::vec3 a = glm::vec3(0, 0, 0);
    glm::vec3 b = glm::vec3(side_length, 0, 0);
    glm::vec3 c = glm::vec3(side_length / 2.0f, 0, -side_length * sqrt(3.0f) / 2.0f);
    glm::vec3 d = glm::vec3(side_length / 2.0f, -side_length * sqrt(6.0f) / 3.0f, side_length * sqrt(3.0f) / 2.0f / 3.0f);

    glm::vec3 center = glm::vec3(side_length / 2.0f, -side_length * sqrt(6.0f) / 3.0f * (3.0f / 4.0f), side_length * sqrt(3.0f) / 2.0f / 3.0f);

    glm::vec3 vertices[] = {
        a,b,c,d,
    };
}

std::vector<Triangle> MySphere::subdivide(Triangle triangle) {
    // TODO 法向量
    glm::vec3 new_vertex0 = (triangle.vertices[0] + triangle.vertices[1]) / 2.0f;
    glm::vec3 new_vertex1 = (triangle.vertices[1] + triangle.vertices[2]) / 2.0f;
    glm::vec3 new_vertex2 = (triangle.vertices[2] + triangle.vertices[0]) / 2.0f;

    Triangle new_triangle0 = { triangle.vertices[0], new_vertex0, new_vertex2 };
    Triangle new_triangle1 = { new_vertex0, triangle.vertices[1], new_vertex1 };
    Triangle new_triangle2 = { new_vertex0, new_vertex1, new_vertex2 };
    Triangle new_triangle3 = { new_vertex2, new_vertex1,  triangle.vertices[2] };

    return { new_triangle0, new_triangle1, new_triangle2, new_triangle3 };
}

void MySphere::create_icosphere(int recursive_depth) {
    m_mesh.reset();
    m_recursive_depth = recursive_depth;

    // 0. 创建正四面体
    float side_length = 1.0f;
    glm::vec3 a = glm::vec3(0, 0, 0);
    glm::vec3 b = glm::vec3(side_length, 0, 0);
    glm::vec3 c = glm::vec3(side_length / 2.0f, 0, -side_length * sqrt(3.0f) / 2.0f);
    glm::vec3 d = glm::vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f, -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    glm::vec3 center = glm::vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f * (1.0f / 4.0f), -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    m_triangles = {
        { a,b,c },
        { a,b,d },
        { b,c,d },
        { c,a,d },
    };

    // 1. 对每个面细分
    for (int i = 0; i < recursive_depth; i++) {
        std::vector<Triangle> new_triangles;
        for (int j = 0; j < m_triangles.size(); j++) {
            auto divided_triangles = subdivide(m_triangles[j]);
            new_triangles.insert(new_triangles.end(), divided_triangles.begin(), divided_triangles.end());
        }
        m_triangles = new_triangles;
    }

    // 2. 对所有细分顶点到中心的距离做归一化
    std::vector<Vertex> all_vertices;
    for (auto& triangle : m_triangles) {
        for (auto& vertex : triangle.vertices) {
            vertex = center + glm::normalize(vertex - center);
            Vertex v;
            v.position = vertex;
            all_vertices.push_back(v);
        }
    }

    m_mesh = Mesh(all_vertices);
}