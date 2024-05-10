#ifndef RenderSourceData_hpp
#define RenderSourceData_hpp

#include "Core/Math.hpp"
#include "ResourceManager/AssetManager.hpp"
#include <Logical/Framework/Object/GObject.hpp>

struct RenderMaterialData {
    unsigned int albedo_map{ 0 };
    unsigned int metallic_map{ 0 };
    unsigned int roughness_map{ 0 };
    unsigned int ao_map{ 0 };
    // temp
    Vec3 albedo{ Vec3(1.0f) };
    float metallic{ 1.0f };
    float roughness{ 0.5f };
    float ao{ 0.01f };

    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    unsigned int normal_map{ 0 };
    unsigned int height_map{ 0 };
    // temp
    float shininess{ 128.0f };
};

struct RenderSubMeshData {
public:
    RenderSubMeshData(const Asset::SubMesh& sub_mesh_asset, const Mat4& model_transform = Mat4(1.0f)) 
        : m_vertices(sub_mesh_asset.sub_mesh_data->vertices())
        , m_indices(sub_mesh_asset.sub_mesh_data->indices()) 
        , m_transform(model_transform * sub_mesh_asset.local_transform)
    {
        updateRenderMaterialData(sub_mesh_asset.material);

        create_vbo();
        create_vao();
    }
    RenderSubMeshData(const RenderSubMeshData& rhs) 
        : m_vertices(rhs.m_vertices)
        , m_indices(rhs.m_indices)
        , m_material(rhs.m_material)
        , m_transform(rhs.m_transform)
    {}
    ~RenderSubMeshData() { reset(); }

    void reset() { if (m_VAO) glDeleteVertexArrays(1, &m_VAO); }
    unsigned int getVAO() const { return m_VAO; }
    size_t indicesCount() const { return m_indices.size(); }
    size_t verticesCount() const { return m_vertices.size(); }
    const RenderMaterialData& renderMaterialData() const { return m_material; }
    const Mat4& transform() const { return m_transform; }
    void updateTransform(const Mat4& transform) { m_transform = transform; }
    void updateRenderMaterialData(const std::shared_ptr<Asset::Material>& material_asset) {
        if (material_asset) {
            // temp
            m_material.albedo = material_asset->albedo;
            m_material.metallic = material_asset->metallic;
            m_material.roughness = material_asset->roughness;
            m_material.ao = material_asset->ao;

            // TODO 贴图更新了，texture数据的释放和加载
            unsigned int diffuse_map = Texture::generate_texture_from_file(material_asset->diffuse_map_filename, false);
            m_material.diffuse_map = diffuse_map;
            unsigned int specular_map = Texture::generate_texture_from_file(material_asset->specular_map_filename, false);
            m_material.specular_map = specular_map;
            unsigned int normal_map = Texture::generate_texture_from_file(material_asset->normal_map_filename, false);
            m_material.normal_map = normal_map;
            unsigned int height_map = Texture::generate_texture_from_file(material_asset->height_map_filename, false);
            m_material.height_map = height_map;
            m_material.shininess = material_asset->shininess;
        }
    }

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
    RenderMaterialData m_material;
    Mat4 m_transform;
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

    std::shared_ptr<RenderSubMeshData> render_sub_mesh_data;
};

struct RenderSkyboxData {
    unsigned int skybox_cube_map;

    std::shared_ptr<RenderSubMeshData> render_sub_mesh_data;
};

struct RenderCameraData {

};

struct RenderSourceData {
    std::vector<std::shared_ptr<RenderSubMeshData>> render_object_sub_mesh_data_list;
    std::vector<RenderDirectionalLightData> render_directional_light_data_list;
    std::vector<RenderPointLightData> render_point_light_data_list;
    RenderSkyboxData render_skybox_data;

    Vec3 camera_position;
    Mat4 view_matrix;
    Mat4 proj_matrix;
    // RenderCameraData
};

#endif // !RenderSourceData_hpp
