#include "MySphere.hpp"
#include <stdio.h>
#include <time.h>

MySphere::MySphere() {
    create_icosphere(6);
}

void MySphere::create_tetrahedron() {
    float side_length = 1.0f;
    glm::vec3 a = glm::vec3(0, 0, 0);
    glm::vec3 b = glm::vec3(side_length, 0, 0);
    glm::vec3 c = glm::vec3(side_length / 2.0f, 0, -side_length * sqrt(3.0f) / 2.0f);
    glm::vec3 d = glm::vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f, -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    m_center = glm::vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f * (1.0f / 4.0f), -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    m_triangles = {
        { c,b,a },
        { a,b,d },
        { b,c,d },
        { c,a,d },
    };
}

std::vector<Triangle> MySphere::subdivide(Triangle triangle) {
    glm::vec3 new_vertex0 = (triangle.vertices[0] + triangle.vertices[1]) / 2.0f;
    glm::vec3 new_vertex1 = (triangle.vertices[1] + triangle.vertices[2]) / 2.0f;
    glm::vec3 new_vertex2 = (triangle.vertices[2] + triangle.vertices[0]) / 2.0f;

    Triangle new_triangle0 = { triangle.vertices[0], new_vertex0, new_vertex2 };
    Triangle new_triangle1 = { new_vertex0, triangle.vertices[1], new_vertex1 };
    Triangle new_triangle2 = { new_vertex0, new_vertex1, new_vertex2 };
    Triangle new_triangle3 = { new_vertex2, new_vertex1, triangle.vertices[2] };

    return { new_triangle0, new_triangle1, new_triangle2, new_triangle3 };
}

void MySphere::create_icosphere(int recursive_depth) {
    m_mesh.reset();
    m_recursive_depth = recursive_depth;

    // 0. 创建正四面体
    create_tetrahedron();

    // Start measuring time
    clock_t start = clock();

    // 1. 对每个面细分
    for (int i = 0; i < recursive_depth; i++) {
        std::vector<Triangle> new_triangles;
        for (int j = 0; j < m_triangles.size(); j++) {
            auto divided_triangles = subdivide(m_triangles[j]);
            new_triangles.insert(new_triangles.end(), divided_triangles.begin(), divided_triangles.end());
        }
        m_triangles = new_triangles;
        clock_t end = clock();
        double elapsed = double(end - start) / CLOCKS_PER_SEC;
        if (elapsed > 1) {
            create_icosphere(recursive_depth - 1);
            return;
        }
    }

    // 2. 对所有细分顶点到中心的距离做归一化
    std::vector<Vertex> all_vertices;
    for (auto& triangle : m_triangles) {
        for (auto& vertex : triangle.vertices) {
            vertex = m_center + glm::normalize(vertex - m_center);
        }
        for (auto& vertex : triangle.vertices) {
            glm::vec3 a = triangle.vertices[0] - triangle.vertices[1];
            glm::vec3 b = triangle.vertices[0] - triangle.vertices[2];
            glm::vec3 normal = glm::normalize(glm::cross(a, b));

            Vertex v;
            v.position = vertex;
            v.normal = normal;
            all_vertices.push_back(v);
        }
    }

    m_mesh = Mesh(all_vertices);

    // Stop measuring time and calculate the elapsed time
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    printf("create_icosphere(%d): Time measured: %.3f seconds.\n", recursive_depth, elapsed);
}