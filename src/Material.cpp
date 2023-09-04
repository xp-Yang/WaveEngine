#include "Material.hpp"
#include "stb_image.h"

void Material::update_shader_binding() const {
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
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    unsigned int pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * nrComponents, 0, GL_STREAM_DRAW);
    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    if (ptr)
    {
        memcpy(ptr, data, width * height * nrComponents);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release the mapped buffer
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
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

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

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
    diffuse_map = generate_texture_from_file(map_path, false);
    diffuse_map_path = map_path;
}

void Material::set_specular_map(std::string map_path)
{
    specular_map = generate_texture_from_file(map_path, false);
    specular_map_path = map_path;
}