#include "Material.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Texture.hpp"

namespace Asset {

void Material::update_shader_binding() const
{
    // TODO 放到render层数据结构去做
    if (shader) {
        shader->start_using();

        shader->setFloat("material.shininess", shininess);

        std::string resource_dir = RESOURCE_DIR;
        static unsigned int default_map = Texture::generate_texture_from_file(resource_dir + "/images/default_map.png", false);

        // temp
        shader->setFloat3("albedo", albedo);
        shader->setFloat("metallic", metallic);
        shader->setFloat("roughness", roughness);
        shader->setFloat("ao", ao);

        shader->setTexture("material.diffuse_map", 0, diffuse_map != 0 ? diffuse_map : default_map);
        shader->setTexture("material.specular_map", 1, specular_map != 0 ? specular_map : default_map);
        shader->setTexture("material.normal_map", 2, normal_map != 0 ? normal_map : default_map);
        shader->setTexture("material.height_map", 3, height_map != 0 ? height_map : default_map);

        shader->stop_using();
    }
}

void Material::set_diffuse_map(std::string map_filename)
{
    std::string resource_dir = RESOURCE_DIR;
    diffuse_map = Texture::generate_texture_from_file(map_filename.c_str(), resource_dir, false);
    diffuse_map_filename = map_filename;
}

void Material::set_specular_map(std::string map_filename)
{
    std::string resource_dir = RESOURCE_DIR;
    specular_map = Texture::generate_texture_from_file(map_filename.c_str(), resource_dir, false);
    specular_map_filename = map_filename;
}

}