#ifndef Material_hpp
#define Material_hpp

#include "Core/Math/Math.hpp"
#include "Texture.hpp"

namespace Asset {

struct Material {
    Material()
        : diffuse_texture(TextureType::Diffuse, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false)
        , specular_texture(TextureType::Specular, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false)
        , normal_texture(TextureType::Normal, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false)
        , height_texture(TextureType::Height, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false)
    {}

    // pbr
    Texture albedo_texture;
    Texture metallic_texture;
    Texture roughness_texture;
    Texture ao_texture;
    // temp
    Vec3 albedo{ Vec3(1.0f) };
    float metallic{ 1.0f };
    float roughness{ 0.5f };
    float ao{ 0.01f };

    // blinn phong
    Texture diffuse_texture;
    Texture specular_texture;
    Texture normal_texture;
    Texture height_texture;

    float alpha{ 1.0f };
};

}

#endif