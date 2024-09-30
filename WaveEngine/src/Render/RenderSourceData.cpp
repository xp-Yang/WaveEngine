#include "RenderSourceData.hpp"
#include "Render/RHI/rhi.hpp"
#include "stb_image.h"
#include "EngineAPI.hpp"

RenderMeshData::RenderMeshData(const RenderMeshDataID& id, const Asset::SubMesh& sub_mesh_asset, const Mat4& model_transform)
    : m_id(id)
    , m_transform(model_transform * sub_mesh_asset.local_transform)
{
    switch (sub_mesh_asset.mesh_file_ref.mesh_file_type) {
    case Asset::MeshFileType::OBJ: {
        Asset::ObjImporter obj_importer;
        obj_importer.load(sub_mesh_asset.mesh_file_ref.mesh_filepath);
        m_mesh_data = obj_importer.meshDataOfNode(sub_mesh_asset.sub_mesh_idx);
        break;
    }
    case Asset::MeshFileType::CustomCube: {
        m_mesh_data = Asset::MeshData::create_cube_mesh();
        break;
    }
    case Asset::MeshFileType::CustomSphere: {
        m_mesh_data = Asset::MeshData::create_icosphere_mesh(0.05f, 4);
        break;
    }
    case Asset::MeshFileType::CustomGround: {
        m_mesh_data = Asset::MeshData::create_complex_quad_mesh(Vec2(1.0f));
        break;
    }
    case Asset::MeshFileType::CustomScreen: {
        m_mesh_data = Asset::MeshData::create_screen_mesh();
        break;
    }
    default:
        break;
    }
    updateRenderMaterialData(sub_mesh_asset.material);
    create();
}

void RenderMeshData::reset()
{
    //if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void RenderMeshData::updateTransform(const Mat4& transform)
{
    m_transform = transform;
}

void RenderMeshData::updateRenderMaterialData(const Asset::Material& material_asset)
{
    // temp
    m_material.albedo = material_asset.albedo;
    m_material.metallic = material_asset.metallic;
    m_material.roughness = material_asset.roughness;
    m_material.ao = material_asset.ao;

    // TODO 贴图更新了，texture数据的释放和加载
    unsigned int diffuse_map = RenderTextureData(material_asset.diffuse_texture).id;
    m_material.diffuse_map = diffuse_map;
    unsigned int specular_map = RenderTextureData(material_asset.specular_texture).id;
    m_material.specular_map = specular_map;
    unsigned int normal_map = RenderTextureData(material_asset.normal_texture).id;
    m_material.normal_map = normal_map;
    unsigned int height_map = RenderTextureData(material_asset.height_texture).id;
    m_material.height_map = height_map;
}

void RenderMeshData::create_instancing(void* instancing_data, int instancing_data_size)
{
    Rhi* rhi = GetApp().renderSystem()->rhi();

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

void RenderMeshData::create()
{
    Rhi* rhi = GetApp().renderSystem()->rhi();

    RhiBuffer* vbuf = rhi->newBuffer(RhiBuffer::Immutable, RhiBuffer::VertexBuffer, (void*)(&(m_mesh_data->vertices()[0])), m_mesh_data->vertices().size() * sizeof(Vertex));
    vbuf->create();
    //glGenBuffers(1, &m_VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //glBufferData(GL_ARRAY_BUFFER, m_mesh_data->vertices().size() * sizeof(Vertex), &(m_mesh_data->vertices()[0]), GL_STATIC_DRAW);

    assert(!m_mesh_data->indices().empty());
    RhiBuffer* ibuf = rhi->newBuffer(RhiBuffer::Immutable, RhiBuffer::IndexBuffer, (void*)(&(m_mesh_data->indices()[0])), m_mesh_data->indices().size() * sizeof(unsigned int));
    ibuf->create();
    //glGenBuffers(1, &m_IBO);
    ////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    ////glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh_data->indices().size() * sizeof(unsigned int), &(m_mesh_data->indices()[0]), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, m_IBO);
    //glBufferData(GL_ARRAY_BUFFER, m_mesh_data->indices().size() * sizeof(unsigned int), &(m_mesh_data->indices()[0]), GL_STATIC_DRAW);

    m_vertex_layout = rhi->newVertexLayout(vbuf, ibuf);
    m_vertex_layout->setAttributes({
        {0, RhiVertexAttribute::Format::Float3, sizeof(Vertex), 0},                            // position
        {1, RhiVertexAttribute::Format::Float3, sizeof(Vertex), offsetof(Vertex, normal)},     // normal
        {2, RhiVertexAttribute::Format::Float2, sizeof(Vertex), offsetof(Vertex, texture_uv)}, // uv
        });
    m_vertex_layout->create();
    m_VAO = m_vertex_layout->id();
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

RenderTextureData::RenderTextureData(const Asset::Texture& texture_asset)
{
    Rhi* rhi = GetApp().renderSystem()->rhi();

    std::string filepath = texture_asset.texture_filepath;
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        RhiTexture::Format format;
        if (nrComponents == 1)
            format = RhiTexture::Format::R8;
        else if (nrComponents == 3)
            format = RhiTexture::Format::RGB16F;
        else if (nrComponents == 4)
            format = RhiTexture::Format::RGBA16F;

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
    //unsigned int textureID;
    //glGenTextures(1, &textureID);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //int width, height, nrChannels;
    //for (unsigned int i = 0; i < cube_texture_asset.cube_texture_filepath.size(); i++)
    //{
    //    unsigned char* data = stbi_load(cube_texture_asset.cube_texture_filepath[i].c_str(), &width, &height, &nrChannels, 0);
    //    if (data)
    //    {
    //        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //        stbi_image_free(data);
    //    }
    //    else
    //    {
    //        assert(false);
    //        stbi_image_free(data);
    //    }
    //}
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //map = textureID;
}

RenderTextureData& RenderTextureData::defaultTexture()
{
    static Asset::Texture diffuse_texture(Asset::TextureType::Custom, std::string(RESOURCE_DIR) + "/images/default_map.png", false);
    static RenderTextureData texture(diffuse_texture);
    return texture;
}
