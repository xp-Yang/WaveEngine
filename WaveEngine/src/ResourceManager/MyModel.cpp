#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MyModel.hpp"
#include "Texture.hpp"

Model::Model(const std::string& file_path)
{
	load_model(file_path);
}

std::string directory;
void Model::load_model(const std::string& file_path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        auto error_str = importer.GetErrorString();
        return;
    }
    directory = file_path.substr(0, file_path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_datas.push_back(load_mesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

SubModelData Model::load_mesh(aiMesh* mesh, const aiScene* scene)
{
    SubModelData sub;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
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

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        sub.material = (load_material(material));
    }
    else {
        sub.material = Material();
        assert(false);
    }

    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    sub.mesh = Mesh(vertices, indices);

    return sub;
}

Material Model::load_material(aiMaterial* material) {
    Material res;

    aiString str;
    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        res.diffuse_map = Texture::generate_texture_from_file(str.C_Str(), directory, false);
        res.diffuse_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR)) {
        material->GetTexture(aiTextureType_SPECULAR, 0, &str);
        res.specular_map = Texture::generate_texture_from_file(str.C_Str(), directory, false);
        res.specular_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_NORMALS)) {
        material->GetTexture(aiTextureType_NORMALS, 0, &str);
        res.normal_map = Texture::generate_texture_from_file(str.C_Str(), directory, false);
        res.normal_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_HEIGHT)) {
        material->GetTexture(aiTextureType_HEIGHT, 0, &str);
        res.height_map = Texture::generate_texture_from_file(str.C_Str(), directory, false);
        res.height_map_path = directory + '/' + std::string(str.C_Str());
    }

    return res;
}