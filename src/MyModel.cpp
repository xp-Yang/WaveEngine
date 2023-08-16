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
        m_meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
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

        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
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

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = load_textures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = load_textures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = load_textures(material, aiTextureType_NORMALS);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = load_textures(material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_textures(aiMaterial* material, aiTextureType type)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        Texture texture;
        texture.id = Mesh::generate_texture_from_file(str.C_Str(), directory, false);
        texture.type = type;
        //texture.path = str;
        textures.push_back(texture);
    }
    return textures;
}

void Model::draw(const Shader& shader, const Renderer& renderer)
{
    shader.start_using();
    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        auto& textures = m_meshes[i].get_texures();
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

            std::string name;
            aiTextureType type = textures[i].type;
            if (type == aiTextureType_DIFFUSE) {
                name = "texture_diffuse";
                name += std::to_string(diffuseNr++);
            }
            else if (type == aiTextureType_SPECULAR) {
                name = "texture_specular";
                name += std::to_string(specularNr++);
            }
            else if (type == aiTextureType_NORMALS) {
                name = "texture_normal";
                name += std::to_string(normalNr++);
            }
            else if (type == aiTextureType_HEIGHT) {
                name = "texture_height";
                name += std::to_string(heightNr++);
            }
            // now set the sampler to the correct texture unit
            shader.setTexture(name, i);
        }
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);

        renderer.draw(shader, m_meshes[i].get_VAO(), DrawMode::Indices, m_meshes[i].get_indices_count(), 0);
    }
}