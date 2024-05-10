#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ObjImporter.hpp"
#include "Texture.hpp"

namespace Asset{

std::shared_ptr<Asset::Mesh> ObjImporter::load(const std::string& file_path)
{
    m_data.reset();
    m_data = std::make_shared<Asset::Mesh>();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        auto error_str = importer.GetErrorString();
        return m_data;
    }
    m_directory = file_path.substr(0, file_path.find_last_of("/\\"));

    processNode(scene->mRootNode, scene);

    return m_data;
}

void ObjImporter::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::shared_ptr<Asset::MeshData> sub_mesh_data = load_sub_mesh_data(mesh, scene);

        // 处理材质
        assert(mesh->mMaterialIndex >= 0);
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::shared_ptr<Asset::Material> sub_mesh_material = load_material(material);

        m_data->sub_meshes.emplace_back(Asset::SubMesh{ sub_mesh_data, sub_mesh_material, Mat4(1.0f) });
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Asset::MeshData> ObjImporter::load_sub_mesh_data(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 处理顶点位置、法线和纹理坐标
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        Vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;

        if (mesh->HasNormals()) {
            Vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertex.normal = normal;
        }

        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            Vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_uv = vec;
        }
        else
            vertex.texture_uv = Vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return std::make_shared<Asset::MeshData>(vertices, indices);
}

std::shared_ptr<Asset::Material> ObjImporter::load_material(aiMaterial* material) {
    std::shared_ptr<Asset::Material> res = std::make_shared<Asset::Material>();

    aiString str;
    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        res->diffuse_map_filename = m_directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR)) {
        material->GetTexture(aiTextureType_SPECULAR, 0, &str);
        res->specular_map_filename = m_directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_NORMALS)) {
        material->GetTexture(aiTextureType_NORMALS, 0, &str);
        res->normal_map_filename = m_directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_HEIGHT)) {
        material->GetTexture(aiTextureType_HEIGHT, 0, &str);
        res->height_map_filename = m_directory + '/' + std::string(str.C_Str());
    }

    return res;
}

}