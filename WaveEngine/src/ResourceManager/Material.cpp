#include "Material.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Texture.hpp"

namespace Asset {

Material::Material()
{
    diffuse_map_filename = std::string(RESOURCE_DIR) + "/images/pure_white_map.png";
    specular_map_filename = std::string(RESOURCE_DIR) + "/images/pure_white_map.png";
    normal_map_filename = std::string(RESOURCE_DIR) + "/images/pure_white_map.png";
    height_map_filename = std::string(RESOURCE_DIR) + "/images/pure_white_map.png";
}

}