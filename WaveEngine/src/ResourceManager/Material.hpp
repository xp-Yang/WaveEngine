#ifndef Material_hpp
#define Material_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"

class Shader;
struct Material {
    //float diffuse_strength{ 1.0f };
    //float specular_strength{ 1.0f };
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
    Vec3 albedo{ Vec3(1.0f) };
    float metallic{ 1.0f };
    float roughness{ 0.5f };
    float ao{ 0.01f };
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

    // ����һ�����е�shader������Դ
    Shader* shader{ nullptr };

    void update_shader_binding() const;
    void set_specular_map(std::string map_path);
    void set_diffuse_map(std::string map_path);
};

#endif