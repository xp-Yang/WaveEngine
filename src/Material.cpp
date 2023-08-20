#include "Material.hpp"
#include "Mesh.hpp"

void Material::update_shader_binding() {
	if (shader) {
        // TODO location不存在则返回-1
        shader->start_using();
        shader->setFloat3("color", color);
        shader->setFloat("material.shininess", shininess);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        shader->setTexture("material.diffuse_map", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);
        shader->setTexture("material.specular_map", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_map);
        shader->setTexture("material.normal_map", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, height_map);
        shader->setTexture("material.height_map", 3);

        shader->setFloat("material.diffuse", diffuse_strength);
        shader->setFloat("material.specular", specular_strength);
	}
}

void Material::set_diffuse_map(std::string map_path)
{
    // TODO 这个函数要放在Mesh里吗？
    diffuse_map = Mesh::generate_texture_from_file(map_path, false);
    diffuse_map_path = map_path;
}

void Material::set_specular_map(std::string map_path)
{
    specular_map = Mesh::generate_texture_from_file(map_path, false);
    specular_map_path = map_path;
}