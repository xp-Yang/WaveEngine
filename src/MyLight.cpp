#include "MyLight.hpp"

MyLight::MyLight(const glm::vec4& color)
{
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    m_model_matrix = scale * glm::mat4(1.0f);

	auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 5.0f, 0.0f });
	m_model_matrix = translate * m_model_matrix;

    create_vbo();
    create_vao();
}

void MyLight::create_vbo()
{
    GLfloat cubeVertex[] =
    {
        // vertex           
        -0.2f, -0.2f,  0.2f, // 0 ����
         0.2f, -0.2f,  0.2f, // 1 ����
         0.2f,  0.2f,  0.2f, // 2 ����
        -0.2f,  0.2f,  0.2f, // 3 ����
        -0.2f, -0.2f, -0.2f, // 4 ������
        -0.2f,  0.2f, -0.2f, // 5 ������
         0.2f,  0.2f, -0.2f, // 6 ������
         0.2f, -0.2f, -0.2f, // 7 ������
    };
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0 ǰ��
        4, 5, 6, 4, 6, 7, // Quad 1 ����
        5, 3, 2, 5, 2, 6, // Quad 2 ����
        4, 7, 1, 4, 1, 0, // Quad 3 ����
        7, 6, 2, 7, 2, 1, // Quad 4 ����
        4, 0, 3, 4, 3, 5  // Quad 5 ����
    };
    glGenBuffers(1, &m_ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyLight::create_vao() {
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    // ��������
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // ��������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());
}