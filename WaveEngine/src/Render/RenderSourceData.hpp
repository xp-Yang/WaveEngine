#ifndef RenderSourceData_hpp
#define RenderSourceData_hpp

#include "Core/Common.hpp"
#include "ResourceManager/AssetManager.hpp"
#include "Logical/Framework/Object/GObject.hpp"
#include "RenderShaderObject.hpp"

using GL_RESOURCE_HANLE = unsigned int;

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
};

struct RenderMeshDataID {
    RenderMeshDataID(GObjectID object_id, int sub_mesh_idx)
        : object_id(object_id)
        , sub_mesh_idx(sub_mesh_idx)
    {}
    bool operator==(const RenderMeshDataID& rhs) const {
        return object_id == rhs.object_id && sub_mesh_idx == rhs.sub_mesh_idx;
    }
    GObjectID object_id;
    int sub_mesh_idx;
};
struct RenderMeshDataIDHasher {
    size_t operator()(const RenderMeshDataID& id) const {
        return (std::hash<int>()(id.object_id.id) ^ (std::hash<int>()(id.sub_mesh_idx)) << 1);
    }
};

class RenderMeshData {
public:
    RenderMeshData(const RenderMeshDataID& id, const Asset::SubMesh& sub_mesh_asset, const Mat4& model_transform = Mat4(1.0f));
    ~RenderMeshData() { reset(); }

    void reset();
    RenderMeshDataID ID() const { return m_id; }
    GL_RESOURCE_HANLE getVAO() const { return m_VAO; }
    size_t indicesCount() const { return m_mesh_data->indices().size(); }
    size_t verticesCount() const { return m_mesh_data->vertices().size(); }
    const RenderMaterialData& renderMaterialData() const { return m_material; }
    const Mat4& transform() const { return m_transform; }
    void updateTransform(const Mat4& transform);
    void updateRenderMaterialData(const Asset::Material& material_asset);

protected:
    void create_vbo();

private:
    GL_RESOURCE_HANLE m_VAO = 0;
    GL_RESOURCE_HANLE m_VBO = 0;
    GL_RESOURCE_HANLE m_IBO = 0;
    std::shared_ptr<Asset::MeshData> m_mesh_data;
    RenderMaterialData m_material;
    Mat4 m_transform;
    RenderMeshDataID m_id;
};


struct RenderDirectionalLightData {
    Color4 color;
    Vec3 direction;
    Mat4 lightViewMatrix;
    Mat4 lightProjMatrix;
};

struct RenderPointLightData {
    Color4 color;
    Vec3 position;
    float radius;
    std::array<Mat4, 6> lightViewMatrix;
    Mat4 lightProjMatrix;

    std::shared_ptr<RenderMeshData> render_sub_mesh_data;
};

struct RenderSkyboxData {
    GL_RESOURCE_HANLE skybox_cube_map;

    std::shared_ptr<RenderMeshData> render_sub_mesh_data;
};

struct RenderCameraData {
    float fov;
    Vec3 pos;
    Vec3 direction;
    Vec3 rightDirection;
    Vec3 upDirection;
};

struct RenderSourceData {
    std::unordered_map<RenderMeshDataID, std::shared_ptr<RenderMeshData>, RenderMeshDataIDHasher> render_mesh_data_hash;
    std::vector<RenderDirectionalLightData> render_directional_light_data_list;
    std::vector<RenderPointLightData> render_point_light_data_list;
    RenderSkyboxData render_skybox_data;

    std::vector<GObjectID> picked_ids;

    Vec3 camera_position;
    Mat4 view_matrix;
    Mat4 proj_matrix;

    std::shared_ptr<RenderCameraData> render_camera;

    void reset() {
        render_mesh_data_hash.clear();
        render_directional_light_data_list.clear();
        render_point_light_data_list.clear();
        render_skybox_data.render_sub_mesh_data.reset();
        picked_ids.clear();
    }
};

#endif // !RenderSourceData_hpp
