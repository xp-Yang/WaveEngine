#ifndef Material_hpp
#define Material_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"
#include "Shader.hpp"

namespace Asset {

struct Material {
    // 引用一个共有的shader程序资源
    Shader* shader{ nullptr };

    // pbr
    unsigned int albedo_map{ 0 };
    unsigned int metallic_map{ 0 };
    unsigned int roughness_map{ 0 };
    unsigned int ao_map{ 0 };
    std::string albedo_map_filename = "";
    std::string metallic_map_filename = "";
    std::string roughness_map_filename = "";
    std::string ao_map_filename = "";
    // temp
    Vec3 albedo{ Vec3(1.0f) };
    float metallic{ 1.0f };
    float roughness{ 0.5f };
    float ao{ 0.01f };

    // blinn phong
    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    unsigned int normal_map{ 0 };
    unsigned int height_map{ 0 };
    std::string diffuse_map_filename = "";
    std::string specular_map_filename = "";
    std::string normal_map_filename = "";
    std::string height_map_filename = "";
    float shininess{ 128.0f };

    void update_shader_binding() const;

    void set_diffuse_map(std::string map_filename);
    void set_specular_map(std::string map_filename);

};

}

#endif