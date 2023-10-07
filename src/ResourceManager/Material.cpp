#include "Material.hpp"
#include "../Platform/OpenGL/Shader.hpp"
#include "Texture.hpp"

void Material::update_shader_binding() const {
	if (shader) {
        // TODO location不存在则返回-1
        shader->start_using();

        shader->setFloat("material.ambient", ambient_strength);
        shader->setFloat("material.shininess", shininess);

        static unsigned int default_map = Texture::generate_texture_from_file("resource/images/default_map.png", false);

        shader->setTexture("material.diffuse_map", 0, diffuse_map != 0 ? diffuse_map : default_map);
        shader->setTexture("material.specular_map", 1, specular_map != 0 ? specular_map : default_map);
        shader->setTexture("material.normal_map", 2, normal_map != 0 ? normal_map : default_map);
        shader->setTexture("material.height_map", 3, height_map != 0 ? height_map : default_map);

        shader->stop_using();
	}
}

void Material::set_diffuse_map(std::string map_path)
{
    diffuse_map = Texture::generate_texture_from_file(map_path, false);
    diffuse_map_path = map_path;
}

void Material::set_specular_map(std::string map_path)
{
    specular_map = Texture::generate_texture_from_file(map_path, false);
    specular_map_path = map_path;
}