#include "RenderSourceData.hpp"
#include "Platform/RHI/rhi.hpp"

void RenderSubMeshData::reset()
{
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void RenderSubMeshData::updateTransform(const Mat4& transform)
{
    m_transform = transform;
}

void RenderSubMeshData::updateRenderMaterialData(const std::shared_ptr<Asset::Material>& material_asset)
{
    if (material_asset) {
        // temp
        m_material.albedo = material_asset->albedo;
        m_material.metallic = material_asset->metallic;
        m_material.roughness = material_asset->roughness;
        m_material.ao = material_asset->ao;

        // TODO 贴图更新了，texture数据的释放和加载
        unsigned int diffuse_map = Asset::Texture::generate_texture_from_file(material_asset->diffuse_map_filename, false);
        m_material.diffuse_map = diffuse_map;
        unsigned int specular_map = Asset::Texture::generate_texture_from_file(material_asset->specular_map_filename, false);
        m_material.specular_map = specular_map;
        unsigned int normal_map = Asset::Texture::generate_texture_from_file(material_asset->normal_map_filename, false);
        m_material.normal_map = normal_map;
        unsigned int height_map = Asset::Texture::generate_texture_from_file(material_asset->height_map_filename, false);
        m_material.height_map = height_map;
        m_material.shininess = material_asset->shininess;
    }
}

void RenderSubMeshData::create_vbo()
{
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderSubMeshData::create_vao()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // 顶点位置数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 顶点法向量数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // 纹理 uv 坐标数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
    glEnableVertexAttribArray(2);

    // 索引数据
    if (!m_indices.empty()) {
        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    }
}
