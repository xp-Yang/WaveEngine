#include "RenderSourceData.hpp"
#include "Render/RHI/rhi.hpp"
#include "stb_image.h"
// TODO remove
#include <glad/glad.h>

RenderMeshData::RenderMeshData(const Asset::SubMesh& sub_mesh_asset)
{
    std::shared_ptr<Asset::MeshData> mesh_data_asset;
    switch (sub_mesh_asset.mesh_file_ref.mesh_file_type) {
    case Asset::MeshFileType::OBJ: {
        Asset::ObjImporter obj_importer;
        obj_importer.load(sub_mesh_asset.mesh_file_ref.mesh_filepath);
        mesh_data_asset = obj_importer.meshDataOfNode(sub_mesh_asset.sub_mesh_idx);
        break;
    }
    case Asset::MeshFileType::CustomCube: {
        mesh_data_asset = Asset::MeshData::create_cube_mesh();
        break;
    }
    case Asset::MeshFileType::CustomSphere: {
        mesh_data_asset = Asset::MeshData::create_icosphere_mesh(0.05f, 4);
        break;
    }
    case Asset::MeshFileType::CustomGround: {
        mesh_data_asset = Asset::MeshData::create_complex_quad_mesh(Vec2(1.0f));
        break;
    }
    case Asset::MeshFileType::CustomScreen: {
        mesh_data_asset = Asset::MeshData::create_screen_mesh();
        break;
    }
    default:
        break;
    }

    m_vertices_count = mesh_data_asset->vertices().size();
    m_indices_count = mesh_data_asset->indices().size();

    const auto& rhi = RenderSourceData::rhi;

    RhiBuffer* vbuf = rhi->newBuffer(RhiBuffer::Immutable, RhiBuffer::VertexBuffer, (void*)(&(mesh_data_asset->vertices()[0])), mesh_data_asset->vertices().size() * sizeof(Vertex));
    vbuf->create();
    //glGenBuffers(1, &m_VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //glBufferData(GL_ARRAY_BUFFER, mesh_data_asset->vertices().size() * sizeof(Vertex), &(mesh_data_asset->vertices()[0]), GL_STATIC_DRAW);

    assert(!mesh_data_asset->indices().empty());
    RhiBuffer* ibuf = rhi->newBuffer(RhiBuffer::Immutable, RhiBuffer::IndexBuffer, (void*)(&(mesh_data_asset->indices()[0])), mesh_data_asset->indices().size() * sizeof(unsigned int));
    ibuf->create();
    //glGenBuffers(1, &m_IBO);
    ////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    ////glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data_asset->indices().size() * sizeof(unsigned int), &(mesh_data_asset->indices()[0]), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, m_IBO);
    //glBufferData(GL_ARRAY_BUFFER, mesh_data_asset->indices().size() * sizeof(unsigned int), &(mesh_data_asset->indices()[0]), GL_STATIC_DRAW);

    m_vertex_layout = rhi->newVertexLayout(vbuf, ibuf);
    m_vertex_layout->setAttributes({
        {0, RhiVertexAttribute::Format::Float3, sizeof(Vertex), 0},                            // position
        {1, RhiVertexAttribute::Format::Float3, sizeof(Vertex), offsetof(Vertex, normal)},     // normal
        {2, RhiVertexAttribute::Format::Float2, sizeof(Vertex), offsetof(Vertex, texture_uv)}, // uv
        });
    m_vertex_layout->create();
    //glGenVertexArrays(1, &m_VAO);
    //glBindVertexArray(m_VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
    //glEnableVertexAttribArray(2);
}

void RenderMeshData::reset()
{
    //if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void RenderMeshData::create_instancing(void* instancing_data, int instancing_data_size)
{
    const auto& rhi = RenderSourceData::rhi;

    RhiBuffer* inst_buf = rhi->newBuffer(RhiBuffer::Immutable, RhiBuffer::VertexBuffer, instancing_data, instancing_data_size);
    inst_buf->create();

    m_vertex_layout->setAttributes({
        {0, RhiVertexAttribute::Format::Float3, sizeof(Vertex), 0},                            // position
        {1, RhiVertexAttribute::Format::Float3, sizeof(Vertex), offsetof(Vertex, normal)},     // normal
        {2, RhiVertexAttribute::Format::Float2, sizeof(Vertex), offsetof(Vertex, texture_uv)}, // uv
        {3, RhiVertexAttribute::Format::Float4, 5 * sizeof(Vec4), 0},
        {4, RhiVertexAttribute::Format::Float4, 5 * sizeof(Vec4), sizeof(Vec4)},
        {5, RhiVertexAttribute::Format::Float4, 5 * sizeof(Vec4), 2 * sizeof(Vec4)},
        {6, RhiVertexAttribute::Format::Float4, 5 * sizeof(Vec4), 3 * sizeof(Vec4)},
        {7, RhiVertexAttribute::Format::Float4, 5 * sizeof(Vec4), 4 * sizeof(Vec4)},
        });
    m_vertex_layout->createInstancing(inst_buf, 3);
}

RenderMaterialData::RenderMaterialData(const Asset::Material& material_asset)
{
    alpha = material_asset.alpha;
    // temp
    albedo = material_asset.albedo;
    metallic = material_asset.metallic;
    roughness = material_asset.roughness;
    ao = material_asset.ao;

    // TODO 贴图更新了，texture数据的释放和加载
    if (diffuse_map == 0) {
        unsigned int diffuse_map = RenderTextureData(material_asset.diffuse_texture).id;
        diffuse_map = diffuse_map;
    }
    if (specular_map == 0) {
        unsigned int specular_map = RenderTextureData(material_asset.specular_texture).id;
        specular_map = specular_map;
    }
    if (normal_map == 0) {
        unsigned int normal_map = RenderTextureData(material_asset.normal_texture).id;
        normal_map = normal_map;
    }
    if (height_map == 0) {
        unsigned int height_map = RenderTextureData(material_asset.height_texture).id;
        height_map = height_map;
    }
}

void RenderMeshNode::updateRenderMaterialData(const Asset::Material& material_asset)
{
    material.alpha = material_asset.alpha;
    // temp
    material.albedo = material_asset.albedo;
    material.metallic = material_asset.metallic;
    material.roughness = material_asset.roughness;
    material.ao = material_asset.ao;

    // TODO 贴图更新了，texture数据的释放和加载
    if (material.diffuse_map == 0) {
        unsigned int diffuse_map = RenderTextureData(material_asset.diffuse_texture).id;
        material.diffuse_map = diffuse_map;
    }
    if (material.specular_map == 0) {
        unsigned int specular_map = RenderTextureData(material_asset.specular_texture).id;
        material.specular_map = specular_map;
    }
    if (material.normal_map == 0) {
        unsigned int normal_map = RenderTextureData(material_asset.normal_texture).id;
        material.normal_map = normal_map;
    }
    if (material.height_map == 0) {
        unsigned int height_map = RenderTextureData(material_asset.height_texture).id;
        material.height_map = height_map;
    }
}

RenderTextureData::RenderTextureData(const Asset::Texture& texture_asset)
{
    const auto& rhi = RenderSourceData::rhi;

    std::string filepath = texture_asset.texture_filepath;
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        RhiTexture::Format format;
        if (nrComponents == 1)
            format = RhiTexture::Format::R8;
        else if (nrComponents == 3)
            format = RhiTexture::Format::RGB8;
        else if (nrComponents == 4)
            format = RhiTexture::Format::RGBA8;

        RhiTexture* texture = rhi->newTexture(format, Vec2(width, height), 1, RhiTexture::Flag::sRGB, data);
        texture->create();

        id = texture->id();

        stbi_image_free(data);
    }
    else
    {
        assert(false);
        stbi_image_free(data);
    }
}

RenderTextureData::RenderTextureData(const Asset::CubeTexture& cube_texture_asset)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < cube_texture_asset.cube_texture_filepath.size(); i++)
    {
        unsigned char* data = stbi_load(cube_texture_asset.cube_texture_filepath[i].c_str(), &width, &height, &nrChannels, 0);
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

    id = textureID;
}

RenderTextureData& RenderTextureData::defaultTexture()
{
    static Asset::Texture diffuse_texture(Asset::TextureType::Custom, std::string(RESOURCE_DIR) + "/images/default_map.png", false);
    static RenderTextureData texture(diffuse_texture);
    return texture;
}