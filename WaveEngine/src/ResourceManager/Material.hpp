#ifndef Material_hpp
#define Material_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"
#include "Shader.hpp"

namespace Asset {

struct Material {
    Material();

    // pbr
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
    std::string diffuse_map_filename = "";
    std::string specular_map_filename = "";
    std::string normal_map_filename = "";
    std::string height_map_filename = "";
    float shininess{ 128.0f };
};

}

#endif