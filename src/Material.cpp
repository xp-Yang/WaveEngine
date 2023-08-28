#include "Material.hpp"
#include "Mesh.hpp"

void Material::update_shader_binding() {
	if (shader) {
        // TODO location不存在则返回-1
        shader->start_using();

        shader->setFloat3("color", color);
        shader->setFloat("material.ambient", ambient_strength);
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
        glActiveTexture(GL_TEXTURE0);

        shader->stop_using();
	}
}


unsigned int generate_texture_from_file(const std::string& full_path, bool gamma) {
    std::string filename = full_path;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        //std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int generate_texture_from_file(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    return generate_texture_from_file(filename, gamma);
}

void Material::set_diffuse_map(std::string map_path)
{
    // TODO 这个函数要放在Mesh里吗？
    diffuse_map = generate_texture_from_file(map_path, false);
    diffuse_map_path = map_path;
}

void Material::set_specular_map(std::string map_path)
{
    specular_map = generate_texture_from_file(map_path, false);
    specular_map_path = map_path;
}