#ifndef Material_hpp
#define Material_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"

class Shader;
struct Material {
    //float diffuse_strength{ 1.0f };
    //float specular_strength{ 1.0f };
    float ambient_strength{ 0.1f };
    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    unsigned int normal_map{ 0 };
    unsigned int height_map{ 0 };
    // pbr
    unsigned int albedo_map{ 0 };
    unsigned int metallic_map{ 0 };
    unsigned int roughness_map{ 0 };
    unsigned int ao_map{ 0 };
    // pbr debug
    Vec3 albedo;
    float metallic;
    float roughness;
    float ao;
        //debug
        std::string diffuse_map_path = "";
        std::string specular_map_path = "";
        std::string normal_map_path = "";
        std::string height_map_path = "";
        std::string albedo_map_path = "";
        std::string metallic_map_path = "";
        std::string roughness_map_path = "";
        std::string ao_map_path = "";
    float shininess{ 128.0f };

    // TODO：Shader 设计考量，是共享其他shader资源，还是自己拥有
    Shader* shader{ nullptr };

    void update_shader_binding() const;
    //void set_shader();
    void set_specular_map(std::string map_path);
    void set_diffuse_map(std::string map_path);
};

#endif