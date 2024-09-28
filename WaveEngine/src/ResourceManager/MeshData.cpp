#include "MeshData.hpp"
#include <windows.h>
#include <iostream>
#include "Core/Logger/Logger.hpp"

namespace Asset {

MeshData::MeshData(const std::vector<Vertex>& vertices, const std::vector<int>& indices)
    : m_vertices(vertices)
    , m_indices(indices)
{
}

void MeshData::reset()
{
    m_vertices.clear();
    m_indices.clear();
}

std::shared_ptr<MeshData> MeshData::create_cube_mesh() {
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    float cubeVertices[] =
    {
        // ���ֻ��λ�ã�ֻ��Ҫ8��������У�����ÿ����ķ�������ͬ��������ͬλ�õĶ�����������ͬ�ķ��������ԣ�һ����Ҫ24��������ͬ�Ķ���
        // pos                  // normal              // uv           
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,    // 0  ���� ���ϣ���ǰ�濴��
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 1  ���� ���£���ǰ�濴��
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,    // 2  ���� ���£���ǰ�濴��
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 3  ���� ���ϣ���ǰ�濴��

        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,    // 4  ǰ�� ����
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 5  ǰ�� ����
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,    // 6  ǰ�� ���� 
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 7  ǰ�� ���� 

        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 8  ����
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 9  ����
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 10 ����
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 11 ����

         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 12 ����
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 13 ����
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 14 ����
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 15 ����

        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,    // 16 ���� ����
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 17 ���� ����
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,    // 18 ���� ����
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 19 ���� ����

        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,    // 20 ���� ����
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 21 ���� ����
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,    // 22 ���� ����
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 23 ���� ����
    };

    for (int i = 0; i < sizeof(cubeVertices) / sizeof(cubeVertices[0]); i += 8) {
        Vertex vertex;

        Vec3 position;
        position.x = cubeVertices[0 + i];
        position.y = cubeVertices[1 + i];
        position.z = cubeVertices[2 + i];
        vertex.position = position;

        Vec3 normal;
        normal.x = cubeVertices[3 + i];
        normal.y = cubeVertices[4 + i];
        normal.z = cubeVertices[5 + i];
        vertex.normal = normal;

        Vec2 vec;
        vec.x = cubeVertices[6 + i];
        vec.y = cubeVertices[7 + i];
        vertex.texture_uv = vec;

        vertices.push_back(vertex);
    }

    int cubeIndices[] =
    {
        1 , 2 , 3 , 1 , 3 , 0 , //����
        5 , 6 , 7 , 5 , 7 , 4 , //ǰ��
        9 , 10, 11, 9 , 11, 8 , //����
        13, 14, 15, 13, 15, 12, //����
        17, 18, 19, 17, 19, 16, //����
        21, 22, 23, 21, 23, 20, //����
    };

    for (int i = 0; i < sizeof(cubeIndices) / sizeof(cubeIndices[0]); i++) {
        indices.push_back(cubeIndices[i]);
    }

    return std::make_shared<MeshData>(vertices, indices);


    // ��ʹ��ibo�ķ�ʽ
    //float cubeVertices[] = {
    //// pos                  // normal              // uv           
    //-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 0  ���� ����   
    // 0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,    // 1  ���� ����   
    // 0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 2  ���� ����   
    // 0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 3  ���� ����   
    //-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,    // 4  ���� ����
    //-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 5  ���� ����
    //                                                                    
    //-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 6  ǰ�� ���� 
    // 0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,    // 7  ǰ�� ���� 
    // 0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 8  ǰ�� ���� 
    // 0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 9  ǰ�� ���� 
    //-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,    // 10 ǰ�� ����
    //-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 11 ǰ�� ����
    //                                                                    
    //-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 12 ���� ����
    //-0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 13 ���� ����
    //-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 14 ���� ����
    //-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 15 ���� ����
    //-0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 16 ���� ����
    //-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 17 ���� ����
    //                                                                    
    // 0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 18 ���� ����
    // 0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 19 ���� ����
    // 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 20 ���� ����
    // 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 21 ���� ����
    // 0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 22 ���� ����
    // 0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 23 ���� ����
    //                                                                    
    //-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 24 ���� ����
    // 0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,    // 25 ���� ����
    // 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 26 ���� ����
    // 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 27 ���� ����
    //-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,    // 28 ���� ����
    //-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 29 ���� ����
    //                                                                    
    //-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 30 ���� ����
    // 0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,    // 31 ���� ����
    // 0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 32 ���� ����
    // 0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 33 ���� ����
    //-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,    // 34 ���� ����
    //-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 35 ���� ����
    //};
    //glGenBuffers(1, &m_vbo_id);
    //glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // û��normal��uvʱ
    //float cubeVertices[] =
    //{
    //    // vertex           
    //    -0.2f, -0.2f,  0.2f, // 0 ����
    //     0.2f, -0.2f,  0.2f, // 1 ����
    //     0.2f,  0.2f,  0.2f, // 2 ����
    //    -0.2f,  0.2f,  0.2f, // 3 ����
    //    -0.2f, -0.2f, -0.2f, // 4 ������
    //    -0.2f,  0.2f, -0.2f, // 5 ������
    //     0.2f,  0.2f, -0.2f, // 6 ������
    //     0.2f, -0.2f, -0.2f, // 7 ������
    //};
    //int cubeIndices[] =
    //{
    //    0, 1, 2, 0, 2, 3, // Quad 0 ǰ��
    //    4, 5, 6, 4, 6, 7, // Quad 1 ����
    //    5, 3, 2, 5, 2, 6, // Quad 2 ����
    //    4, 7, 1, 4, 1, 0, // Quad 3 ����
    //    7, 6, 2, 7, 2, 1, // Quad 4 ����
    //    4, 0, 3, 4, 3, 5  // Quad 5 ����
    //};
}

static void create_tetrahedron(std::vector<Triangle>& triangles, Vec3& center) {
    float side_length = 1.0f;
    Vec3 a = Vec3(0, 0, 0);
    Vec3 b = Vec3(side_length, 0, 0);
    Vec3 c = Vec3(side_length / 2.0f, 0, -side_length * sqrt(3.0f) / 2.0f);
    Vec3 d = Vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f, -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    center = Vec3(side_length / 2.0f, side_length * sqrt(6.0f) / 3.0f * (1.0f / 4.0f), -side_length * sqrt(3.0f) / 2.0f / 3.0f);

    a -= center;
    b -= center;
    c -= center;
    d -= center;
    center -= center;

    triangles = {
        Triangle(c,b,a),
        Triangle(a,b,d),
        Triangle(b,c,d),
        Triangle(c,a,d),
    };
}

static std::vector<Triangle> subdivide(Triangle triangle) {
    Vec3 new_vertex0 = (triangle.vertices[0].position + triangle.vertices[1].position) / 2.0f;
    Vec3 new_vertex1 = (triangle.vertices[1].position + triangle.vertices[2].position) / 2.0f;
    Vec3 new_vertex2 = (triangle.vertices[2].position + triangle.vertices[0].position) / 2.0f;

    Triangle new_triangle0 = Triangle(triangle.vertices[0].position, new_vertex0, new_vertex2);
    Triangle new_triangle1 = Triangle(new_vertex0, triangle.vertices[1].position, new_vertex1);
    Triangle new_triangle2 = Triangle(new_vertex0, new_vertex1, new_vertex2);
    Triangle new_triangle3 = Triangle(new_vertex2, new_vertex1, triangle.vertices[2].position);

    return { new_triangle0, new_triangle1, new_triangle2, new_triangle3 };
}

static std::vector<Triangle> recursive_subdivide(const Triangle& triangle, int recursive_depth) {
    if (recursive_depth == 0) {
        return { triangle };
    }

    Vec3 new_vertex0 = (triangle.vertices[0].position + triangle.vertices[1].position) / 2.0f;
    Vec3 new_vertex1 = (triangle.vertices[1].position + triangle.vertices[2].position) / 2.0f;
    Vec3 new_vertex2 = (triangle.vertices[2].position + triangle.vertices[0].position) / 2.0f;

    Triangle new_triangle0 = Triangle(triangle.vertices[0].position, new_vertex0, new_vertex2);
    Triangle new_triangle1 = Triangle(new_vertex0, triangle.vertices[1].position, new_vertex1);
    Triangle new_triangle2 = Triangle(new_vertex0, new_vertex1, new_vertex2);
    Triangle new_triangle3 = Triangle(new_vertex2, new_vertex1, triangle.vertices[2].position);

    std::vector<Triangle> ret;
    recursive_depth--;
    const std::vector<Triangle>& ret1 = recursive_subdivide(new_triangle0, recursive_depth);
    const std::vector<Triangle>& ret2 = recursive_subdivide(new_triangle1, recursive_depth);
    const std::vector<Triangle>& ret3 = recursive_subdivide(new_triangle2, recursive_depth);
    const std::vector<Triangle>& ret4 = recursive_subdivide(new_triangle3, recursive_depth);
    ret.insert(ret.end(), ret1.begin(), ret1.end());
    ret.insert(ret.end(), ret2.begin(), ret2.end());
    ret.insert(ret.end(), ret3.begin(), ret3.end());
    ret.insert(ret.end(), ret4.begin(), ret4.end());

    return ret;
}

std::shared_ptr<MeshData> MeshData::create_icosphere_mesh(float radius, int regression_depth) {
    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);

    std::vector<Triangle> m_triangles;
    Vec3 m_center;

    // 0. ������������
    create_tetrahedron(m_triangles, m_center);

    // 1. ��ÿ����ϸ��
    for (int i = 0; i < regression_depth; i++) {
        std::vector<Triangle> new_triangles;
        for (int j = 0; j < m_triangles.size(); j++) {
            auto divided_triangles = subdivide(m_triangles[j]);
            new_triangles.insert(new_triangles.end(), divided_triangles.begin(), divided_triangles.end());
        }
        m_triangles = new_triangles;
    }

    //std::vector<Triangle> new_triangles;
    //for (int i = 0; i < m_triangles.size(); i++) {
    //    const auto& ret = recursive_subdivide(m_triangles[i], regression_depth);
    //    new_triangles.insert(new_triangles.end(), ret.begin(), ret.end());
    //}
    //m_triangles = new_triangles;

    // 2. ������ϸ�ֶ��㵽���ĵľ�������һ��
    std::vector<Vertex> all_vertices;
    for (auto& triangle : m_triangles) {
        for (auto& vertex : triangle.vertices) {
            vertex.position = m_center + radius * Normalize(vertex.position - m_center);
        }
        for (auto& vertex : triangle.vertices) {
            //Vec3 a = triangle.vertices[0] - triangle.vertices[1];
            //Vec3 b = triangle.vertices[0] - triangle.vertices[2];
            //Vec3 normal = Normalize(Cross(a, b));
            Vec3 normal = Normalize(vertex.position - m_center);

            Vertex v;
            v.position = vertex.position;
            v.normal = normal;
            all_vertices.push_back(v);
        }
    }

    // TODO optimize index method
    std::vector<int> indices;
    indices.reserve(all_vertices.size() + 1);
    for (int i = 0; i < all_vertices.size(); i++) {
        indices.push_back(i);
    }

    QueryPerformanceCounter(&t2);
    auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;

    Logger::info("MeshData::create_icosphere_mesh({}), time:{}", regression_depth, time);

    return std::make_shared<MeshData>(all_vertices, indices);
}

std::shared_ptr<MeshData> MeshData::create_quad_mesh(const Point3& origin, const Vec3& positive_dir_u, const Vec3& positive_dir_v)
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    Vec3 normal = Normalize(Cross(positive_dir_u, positive_dir_v));
    Point3 origin_p = origin;
    Point3 right_p = origin_p + positive_dir_u;
    Point3 upper_right_p = origin_p + positive_dir_u + positive_dir_v;
    Point3 upper_p = origin_p + positive_dir_v;
    Point3 points[4] = { origin_p, right_p, upper_right_p, upper_p };

    for (int i = 0; i < 4; i++) {
        Vertex vertex;

        vertex.position = points[i];
        vertex.normal = normal;
        float u = (i == 0 || i == 3) ? 0.0f : 1.0f;
        float v = (i == 0 || i == 1) ? 0.0f : 1.0f;
        vertex.texture_uv = { u, v };

        vertices.push_back(vertex);
    }

    int cubeIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    for (int i = 0; i < sizeof(cubeIndices) / sizeof(cubeIndices[0]); i++) {
        indices.push_back(cubeIndices[i]);
    }

    return std::make_shared<MeshData>(vertices, indices);
}

std::shared_ptr<MeshData> MeshData::create_complex_quad_mesh(const Vec2& size)
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    Point3 start_point = Point3(-size.x / 2.0f, 0.0f, size.y / 2.0f);
    Vec3 u = Vec3(-2.0f * start_point.x, 0, 0);
    Vec3 v = Vec3(0, 0, -2.0f * start_point.z);
    Point3 end_point = start_point + u + v;

    size_t sub_quad_num_u = (size_t)(size.x);
    size_t sub_quad_num_v = (size_t)(size.y);
    Vec3 sub_u = u / (float)sub_quad_num_u;
    Vec3 sub_v = v / (float)sub_quad_num_v;
    for (int i = 0; i < sub_quad_num_v; i++) {
        for (int j = 0; j < sub_quad_num_u; j++)
        {
            Point3 sub_start_point = start_point + (float)j * sub_u + (float)i * sub_v;
            Point3 sub_end_point = sub_start_point + sub_u + sub_v;
            std::shared_ptr<MeshData> sub_mesh_data = create_quad_mesh(sub_start_point, sub_u, sub_v);
            vertices.insert(vertices.end(), sub_mesh_data->m_vertices.begin(), sub_mesh_data->m_vertices.end());
            for (auto& index : sub_mesh_data->m_indices) {
                index += 4 * (j + i * sub_quad_num_u);
            }
            indices.insert(indices.end(), sub_mesh_data->m_indices.begin(), sub_mesh_data->m_indices.end());
            sub_mesh_data.reset();
        }
    }

    return std::make_shared<MeshData>(vertices, indices);
}

std::shared_ptr<MeshData> MeshData::create_screen_mesh()
{
    return create_quad_mesh(Point3(-1.0f, -1.0f, 0.0f), Vec3(2.0f, 0.0f, 0.0f), Vec3(0.0f, 2.0f, 0.0f));
}

}