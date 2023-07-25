#include "MyGround.hpp"

MyGround::MyGround(const glm::vec4& color)
    : MyCube()
{
    m_color = color;

    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(10000.0f));
    m_model_matrix = scale * m_model_matrix;

    auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, -1.0f, 0.0f });
    m_model_matrix = translate * m_model_matrix;

    create_vbo();
    create_vao();
}

void MyGround::create_vbo()
{
    GLfloat cubeVertex[] =
    {
        // vertex           
        -1.0f,  0.0f,  -1.0f, // 0
        -1.0f,  0.0f,   1.0f, // 1
         1.0f,  0.0f,   1.0f, // 2
         1.0f,  0.0f,  -1.0f, // 3
    };
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint cubeIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    glGenBuffers(1, &m_ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGround::create_vao() {
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    // 顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());
}