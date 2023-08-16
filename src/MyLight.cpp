#include "MyLight.hpp"

MyLight::MyLight()
{
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    m_model_matrix = scale * glm::mat4(1.0f);

	auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 5.0f, 0.0f });
	m_model_matrix = translate * m_model_matrix;

    init_mesh();
}

void MyLight::init_mesh()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLfloat cubeVertices[] =
    {
        // vertex           
        -0.2f, -0.2f,  0.2f, // 0 左下
         0.2f, -0.2f,  0.2f, // 1 右下
         0.2f,  0.2f,  0.2f, // 2 右上
        -0.2f,  0.2f,  0.2f, // 3 左上
        -0.2f, -0.2f, -0.2f, // 4 后：左下
        -0.2f,  0.2f, -0.2f, // 5 后：左上
         0.2f,  0.2f, -0.2f, // 6 后：右上
         0.2f, -0.2f, -0.2f, // 7 后：右下
    };
    for (int i = 0; i < sizeof(cubeVertices) / sizeof(cubeVertices[0]); i += 3) {
        Vertex vertex;

        glm::vec3 position;
        position.x = cubeVertices[0 + i];
        position.y = cubeVertices[1 + i];
        position.z = cubeVertices[2 + i];
        vertex.position = position;

        //glm::vec3 normal;
        //normal.x = cubeVertices[3 + i];
        //normal.y = cubeVertices[4 + i];
        //normal.z = cubeVertices[5 + i];
        //vertex.normal = normal;

        //glm::vec2 vec;
        //vec.x = cubeVertices[6 + i];
        //vec.y = cubeVertices[7 + i];
        //vertex.texture_uv = vec;

        vertices.push_back(vertex);
    }

    GLuint cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0 前面
        4, 5, 6, 4, 6, 7, // Quad 1 后面
        5, 3, 2, 5, 2, 6, // Quad 2 上面
        4, 7, 1, 4, 1, 0, // Quad 3 下面
        7, 6, 2, 7, 2, 1, // Quad 4 右面
        4, 0, 3, 4, 3, 5  // Quad 5 左面
    };
    for (int i = 0; i < sizeof(cubeIndices) / sizeof(cubeIndices[0]); i++) {
        indices.push_back(cubeIndices[i]);
    }

    m_mesh = Mesh(vertices, indices);
}