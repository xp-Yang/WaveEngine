#include "Logical/Material.hpp"

std::shared_ptr<Material> Material::create_complete_default_material()
{
	auto default_material = std::make_shared<Material>();
    default_material->diffuse_texture = std::make_shared<Texture>(TextureType::Diffuse, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->specular_texture = std::make_shared<Texture>(TextureType::Specular, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->normal_texture = std::make_shared<Texture>(TextureType::Normal, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->height_texture = std::make_shared<Texture>(TextureType::Height, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->albedo_texture = std::make_shared<Texture>(TextureType::Albedo, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->metallic_texture = std::make_shared<Texture>(TextureType::Metallic, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->roughness_texture = std::make_shared<Texture>(TextureType::Roughness, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
    default_material->ao_texture = std::make_shared<Texture>(TextureType::AO, std::string(RESOURCE_DIR) + "/images/pure_white_map.png", false);
	return default_material;
}
