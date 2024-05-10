#ifndef RenderSourceData_hpp
#define RenderSourceData_hpp

#include "Core/Math.hpp"
#include "ResourceManager/AssetManager.hpp"
#include <Logical/Framework/Object/GObject.hpp>

struct RenderTextureData {
    RenderTextureData(const std::string& texture_filepath, bool gamma = false);
    RenderTextureData(std::array<std::string, 6> cube_texture_filepath);

    unsigned int map;
};

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

    void reset();
    unsigned int getVAO() const { return m_VAO; }
    size_t indicesCount() const { return m_indices.size(); }
    size_t verticesCount() const { return m_vertices.size(); }
    const RenderMaterialData& renderMaterialData() const { return m_material; }
    const Mat4& transform() const { return m_transform; }
    void updateTransform(const Mat4& transform);
    void updateRenderMaterialData(const std::shared_ptr<Asset::Material>& material_asset);

protected:
    void create_vbo();
    void create_vao();

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_IBO = 0;
    const std::vector<Vertex>& m_vertices;
    const std::vector<unsigned int>& m_indices;
    RenderMaterialData m_material;
    Mat4 m_transform;
};

//struct RenderShaderObject {
//    Asset::ShaderType shader_asset_type;
//    std::shared_ptr<Asset::Shader> shader_asset;
//};


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
