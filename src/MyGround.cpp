#include "MyGround.hpp"

MyGround::MyGround()
    : Object()
{
    init_meshes();
}

void MyGround::init_meshes()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLfloat cubeVertices[] =
    {
        // vertex               // normal            // uv
        -1.0f,  0.0f,  -1.0f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,// 0
        -1.0f,  0.0f,   1.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,// 1
         1.0f,  0.0f,   1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,// 2
         1.0f,  0.0f,  -1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,// 3
    };
    for (int i = 0; i < sizeof(cubeVertices) / sizeof(cubeVertices[0]); i += 8) {
        Vertex vertex;

        glm::vec3 position;
        position.x = cubeVertices[0 + i];
        position.y = cubeVertices[1 + i];
        position.z = cubeVertices[2 + i];
        vertex.position = position;

        glm::vec3 normal;
        normal.x = cubeVertices[3 + i];
        normal.y = cubeVertices[4 + i];
        normal.z = cubeVertices[5 + i];
        vertex.normal = normal;

        glm::vec2 vec;
        vec.x = cubeVertices[6 + i] * 2.0f;
        vec.y = cubeVertices[7 + i] * 2.0f;
        vertex.texture_uv = vec;

        vertices.push_back(vertex);
    }

    GLuint cubeIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    for (int i = 0; i < sizeof(cubeIndices) / sizeof(cubeIndices[0]); i++) {
        indices.push_back(cubeIndices[i]);
    }

    append_mesh(Mesh(vertices, indices));
}