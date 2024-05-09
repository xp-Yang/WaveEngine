#ifndef RenderSourceData_hpp
#define RenderSourceData_hpp

#include "Core/Math.hpp"
#include "ResourceManager/AssetManager.hpp"
#include <Logical/Framework/Object/GObject.hpp>

struct RenderSubMeshData {
public:
    RenderSubMeshData() = default;
    RenderSubMeshData(const Asset::SubMesh& sub_mesh_asset) 
        : m_vertices(sub_mesh_asset.sub_mesh_data->vertices())
        , m_indices(sub_mesh_asset.sub_mesh_data->indices()) 
        , m_material(sub_mesh_asset.material)
        , m_local_transform(sub_mesh_asset.transform)
    {
        create_vbo();
        create_vao();
    }
    RenderSubMeshData(const RenderSubMeshData& rhs) 
        : m_vertices(rhs.m_vertices)
        , m_indices(rhs.m_indices)
        , m_material(rhs.m_material)
        , m_local_transform(rhs.m_local_transform)
    {}
    ~RenderSubMeshData() { reset(); }

    void reset() { if (m_VAO) glDeleteVertexArrays(1, &m_VAO); }
    unsigned int getVAO() const { return m_VAO; }
    size_t indicesCount() const { return m_indices.size(); }
    size_t verticesCount() const { return m_vertices.size(); }
    const std::shared_ptr<Asset::Material>& material() const { return m_material; }
    const Mat4& localTransform() const { return m_local_transform; }

protected:
    void create_vbo() {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void create_vao() {
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

protected:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_IBO = 0;
    const std::vector<Vertex>& m_vertices;
    const std::vector<unsigned int>& m_indices;
    std::shared_ptr<Asset::Material> m_material;
    Mat4 m_local_transform;
};

struct RenderMeshData {
public:
    RenderMeshData() = default;
    RenderMeshData(const GObject& gobject)
    {
    }
    RenderMeshData(const Asset::Mesh& mesh_asset, const Mat4& transform) 
        : RenderMeshData(mesh_asset.sub_meshes, transform)
    {
    }
    RenderMeshData(const std::vector<Asset::SubMesh>& sub_mesh_asset_list, const Mat4& transform)
    {
        for (const auto& sub_mesh_asset : sub_mesh_asset_list) {
            render_sub_mesh_data_list.emplace_back(sub_mesh_asset);
        }
        model_matrix = transform;
    }

    std::vector<RenderSubMeshData> render_sub_mesh_data_list;
    Mat4 model_matrix;
};

struct RenderDirectionalLightData {
    Color4 color;
    Vec3 direction;
    Mat4 lightReferenceMatrix;
};

struct RenderPointLightData {
    Color4 color;
    Vec3 position;
    float radius;
    std::array<Mat4, 6> lightReferenceMatrix;

    RenderMeshData render_mesh_data;
};

struct RenderSkyboxData {
    unsigned int skybox_cube_map;

    RenderMeshData render_mesh_data;
};

struct RenderCameraData {

};

struct RenderSourceData {
    std::vector<RenderMeshData> render_mesh_data_list;

    std::vector<RenderDirectionalLightData> render_directional_light_data_list;
    std::vector<RenderPointLightData> render_point_light_data_list;

    RenderSkyboxData render_skybox_data;

    Vec3 camera_position;
    Mat4 view_matrix;
    Mat4 proj_matrix;
    // RenderCameraData
};

#endif // !RenderSourceData_hpp
