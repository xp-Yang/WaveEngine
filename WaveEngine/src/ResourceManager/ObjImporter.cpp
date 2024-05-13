#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ObjImporter.hpp"

namespace Asset{

std::unordered_map<std::string, Assimp::Importer*> ObjImporter::m_importers;

ObjImporter::~ObjImporter()
{
}

bool ObjImporter::load(const std::string& file_path)
{
    m_obj_filepath = file_path;
    m_directory = file_path.substr(0, file_path.find_last_of("/\\"));

    if (ObjImporter::m_importers.find(file_path) != ObjImporter::m_importers.end()) {
        m_scene = ObjImporter::m_importers.at(file_path)->GetScene();
    }
    else {
        auto importer = new Assimp::Importer();
        m_scene = importer->ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
        if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
        {
            auto error_str = importer->GetErrorString();
            delete importer;
            return false;
        }
        ObjImporter::m_importers.insert({file_path, importer });
    }

    collect_ai_meshes();

    return true;
}

std::shared_ptr<Asset::MeshData> ObjImporter::meshDataOfNode(int ai_mesh_idx)
{
    auto ai_mesh = m_ai_meshes[ai_mesh_idx];
    return load_sub_mesh_data(ai_mesh);
}

Asset::Material ObjImporter::materialOfNode(int ai_mesh_idx)
{
    auto ai_mesh = m_ai_meshes[ai_mesh_idx];
    assert(ai_mesh->mMaterialIndex >= 0);
    aiMaterial* material = m_scene->mMaterials[ai_mesh->mMaterialIndex];
    return load_material(material);
}

std::vector<aiMesh*> ObjImporter::collect_ai_meshes()
{
    std::vector<aiNode*> nodes{ m_scene->mRootNode };
    while (!nodes.empty()) {
        aiNode* node = nodes.front();
        nodes.erase(nodes.begin());
        for (int i = 0; i < node->mNumMeshes; i++) {
            m_ai_meshes_ids.push_back(node->mMeshes[i]);
            m_ai_meshes.push_back(m_scene->mMeshes[node->mMeshes[i]]);
        }
        for (int i = 0; i < node->mNumChildren; i++) {
            nodes.push_back(node->mChildren[i]);
        }
    }
    return m_ai_meshes;
}

std::shared_ptr<Asset::MeshData> ObjImporter::load_sub_mesh_data(aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    // 处理顶点位置、法线和纹理坐标
    for (int i = 0; i < mesh->mNumVertices; i++)
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
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    return std::make_shared<Asset::MeshData>(vertices, indices);
}

Asset::Material ObjImporter::load_material(aiMaterial* material) {
    Asset::Material res;

    aiString str;
    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        res.diffuse_texture = Asset::Texture(TextureType::Diffuse, m_directory + '/' + std::string(str.C_Str()), false);
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR)) {
        material->GetTexture(aiTextureType_SPECULAR, 0, &str);
        res.specular_texture = Asset::Texture(TextureType::Specular, m_directory + '/' + std::string(str.C_Str()), false);
    }

    if (material->GetTextureCount(aiTextureType_NORMALS)) {
        material->GetTexture(aiTextureType_NORMALS, 0, &str);
        res.normal_texture = Asset::Texture(TextureType::Normal, m_directory + '/' + std::string(str.C_Str()), false);
    }

    if (material->GetTextureCount(aiTextureType_HEIGHT)) {
        material->GetTexture(aiTextureType_HEIGHT, 0, &str);
        res.height_texture = Asset::Texture(TextureType::Height, m_directory + '/' + std::string(str.C_Str()), false);
    }

    return res;
}

}