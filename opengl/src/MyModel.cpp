#include "MyModel.hpp"

Model::Model(const std::string& file_path)
{
	load_model(file_path);
    m_renderer = MyRenderer();
}

void Model::load_model(const std::string& file_path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        auto error_str = importer.GetErrorString();
        return;
    }
    auto directory = file_path.substr(0, file_path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::draw(const MyShader& shader)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        m_renderer.draw(shader, m_meshes[i].get_VAO(), DrawMode::Indices, m_meshes[i].get_indices_count());
    }
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
    //std::vector<Texture> textures;

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

        //if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        //{
        //    glm::vec2 vec;
        //    vec.x = mesh->mTextureCoords[0][i].x;
        //    vec.y = mesh->mTextureCoords[0][i].y;
        //    vertex.TexCoords = vec;
        //}
        //else
        //    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
        ;
    }

    //return Mesh(vertices, indices, textures);
    return Mesh(vertices, indices);
}
