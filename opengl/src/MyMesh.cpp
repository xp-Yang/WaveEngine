#include "MyMesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : m_vertices(vertices)
    , m_indices(indices)
{
    create_vbo();
    create_vao();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : m_vertices (vertices)
    , m_indices (indices)
    , m_textures (textures)
{
    create_vbo();
    create_vao();
}

void Mesh::create_vbo()
{
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::create_vao()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // 顶点位置数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //// 顶点颜色数据
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    // 顶点法向量数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    //// 纹理数据
    //glBindTexture(GL_TEXTURE_2D, get_texture_id());
    //for (unsigned int i = 0; i < m_textures.size(); i++) {
    //    glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    //}
    // 纹理 uv 坐标数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
    glEnableVertexAttribArray(2);

    // 索引数据
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
}
