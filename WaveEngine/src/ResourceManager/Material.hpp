#ifndef Material_hpp
#define Material_hpp

#include "Core/Math.hpp"

namespace Asset {

struct Material {
    Material()
        : diffuse_map_filename(std::string(RESOURCE_DIR) + "/images/pure_white_map.png")
        , specular_map_filename(std::string(RESOURCE_DIR) + "/images/pure_white_map.png")
        , normal_map_filename(std::string(RESOURCE_DIR) + "/images/pure_white_map.png")
        , height_map_filename(std::string(RESOURCE_DIR) + "/images/pure_white_map.png")
    {}

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