#include "MyModel.hpp"
#include "stb_image.h"

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
        m_meshes.push_back(load_mesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::load_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // ������λ�á����ߺ���������
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;

        if (mesh->HasNormals()) {
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertex.normal = normal;
        }

        if (mesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_uv = vec;
        }
        else
            vertex.texture_uv = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // �������
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        m_materials.push_back(load_material(material));
    }
    else {
        assert(false, "there is mesh has no material");
    }

    // ��������
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices);
}

Material Model::load_material(aiMaterial* material) {
    Material res;

    aiString str;
    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        res.diffuse_map = Mesh::generate_texture_from_file(str.C_Str(), directory, false);
        res.diffuse_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR)) {
        material->GetTexture(aiTextureType_SPECULAR, 0, &str);
        res.specular_map = Mesh::generate_texture_from_file(str.C_Str(), directory, false);
        res.specular_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_NORMALS)) {
        material->GetTexture(aiTextureType_NORMALS, 0, &str);
        res.normal_map = Mesh::generate_texture_from_file(str.C_Str(), directory, false);
        res.normal_map_path = directory + '/' + std::string(str.C_Str());
    }

    if (material->GetTextureCount(aiTextureType_HEIGHT)) {
        material->GetTexture(aiTextureType_HEIGHT, 0, &str);
        res.height_map = Mesh::generate_texture_from_file(str.C_Str(), directory, false);
        res.height_map_path = directory + '/' + std::string(str.C_Str());
    }

    return res;
}

void Model::draw(const Shader& shader, const Renderer& renderer)
{
    shader.start_using();
    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        if (m_materials[i].diffuse_map != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_materials[i].diffuse_map);
            shader.setTexture("material.diffuse_map", 0);
        }
        if (m_materials[i].specular_map != 0) {
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, m_materials[i].specular_map);
            shader.setTexture("material.specular_map", 1);
        }
        if (m_materials[i].normal_map != 0) {
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, m_materials[i].normal_map);
            shader.setTexture("material.normal_map", 2);
        }
        if (m_materials[i].height_map != 0) {
            glActiveTexture(GL_TEXTURE0 + 3);
            glBindTexture(GL_TEXTURE_2D, m_materials[i].normal_map);
            shader.setTexture("material.height_map", 3);
        }
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);

        renderer.draw(shader, m_meshes[i].get_VAO(), DrawMode::Indices, m_meshes[i].get_indices_count(), 0);
    }
}