#ifndef RenderSourceData_hpp
#define RenderSourceData_hpp

#include "Core/Math.hpp"
#include "ResourceManager/AssetManager.hpp"
#include <Logical/Framework/Object/GObject.hpp>
#include "RenderShaderObject.hpp"

using GL_RESOURCE_HANLE = unsigned int;

struct RenderTextureData {
    RenderTextureData(const Asset::Texture& texture_asset);
    RenderTextureData(const Asset::CubeTexture& cube_texture_asset);

    GL_RESOURCE_HANLE map;
};

struct RenderMaterialData {
    GL_RESOURCE_HANLE albedo_map{ 0 };
    GL_RESOURCE_HANLE metallic_map{ 0 };
    GL_RESOURCE_HANLE roughness_map{ 0 };
    GL_RESOURCE_HANLE ao_map{ 0 };
    // temp
    Vec3 albedo{ Vec3(1.0f) };
    float metallic{ 1.0f };
    float roughness{ 0.5f };
    float ao{ 0.01f };

    GL_RESOURCE_HANLE diffuse_map{ 0 };
    GL_RESOURCE_HANLE specular_map{ 0 };
    GL_RESOURCE_HANLE normal_map{ 0 };
    GL_RESOURCE_HANLE height_map{ 0 };
    // temp
    float shininess{ 128.0f };
};

struct RenderSubMeshData {
public:
    RenderSubMeshData(const Asset::SubMesh& sub_mesh_asset, const Mat4& model_transform = Mat4(1.0f));
    ~RenderSubMeshData() { reset(); }

    void reset();
    GL_RESOURCE_HANLE getVAO() const { return m_VAO; }
    size_t indicesCount() const { return m_mesh_data->indices().size(); }
    size_t verticesCount() const { return m_mesh_data->vertices().size(); }
    const RenderMaterialData& renderMaterialData() const { return m_material; }
    const Mat4& transform() const { return m_transform; }
    void updateTransform(const Mat4& transform);
    void updateRenderMaterialData(const Asset::Material& material_asset);

protected:
    void create_vbo();
    void create_vao();

private:
    GL_RESOURCE_HANLE m_VAO = 0;
    GL_RESOURCE_HANLE m_VBO = 0;
    GL_RESOURCE_HANLE m_IBO = 0;
    std::shared_ptr<Asset::MeshData> m_mesh_data;
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
    GL_RESOURCE_HANLE skybox_cube_map;

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
