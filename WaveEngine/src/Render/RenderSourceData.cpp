#include "RenderSourceData.hpp"
#include "Platform/RHI/rhi.hpp"
#include "stb/stb_image.h"

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
        unsigned int diffuse_map = RenderTextureData(material_asset->diffuse_map_filename, false).map;
        m_material.diffuse_map = diffuse_map;
        unsigned int specular_map = RenderTextureData(material_asset->specular_map_filename, false).map;
        m_material.specular_map = specular_map;
        unsigned int normal_map = RenderTextureData(material_asset->normal_map_filename, false).map;
        m_material.normal_map = normal_map;
        unsigned int height_map = RenderTextureData(material_asset->height_map_filename, false).map;
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

RenderTextureData::RenderTextureData(const std::string& texture_filepath, bool gamma)
{
    std::string filename = texture_filepath;
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    //unsigned int pbo;
    //glGenBuffers(1, &pbo);
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    //glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * nrComponents, 0, GL_STREAM_DRAW);
    //GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    //if (ptr)
    //{
    //    memcpy(ptr, data, width * height * nrComponents);
    //    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release the mapped buffer
    //}

    unsigned int textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // TODO 这些gl函数都封装进rhi
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        assert(false);
        stbi_image_free(data);
    }

    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    map = textureID;
}

RenderTextureData::RenderTextureData(std::array<std::string, 6> cube_texture_filepath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < cube_texture_filepath.size(); i++)
    {
        unsigned char* data = stbi_load(cube_texture_filepath[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            assert(false);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    map = textureID;
}
