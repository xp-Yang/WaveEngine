#ifndef Material_hpp
#define Material_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/material.h>
#include <string>
#include <vector>
#include "Shader.hpp"

//struct Texture {
//    unsigned int id;
//    aiTextureType type;
//    //aiString path;
//};

struct Material {
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    //float diffuse_strength{ 1.0f };
    //float specular_strength{ 1.0f };
    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    unsigned int normal_map{ 0 };
    unsigned int height_map{ 0 };
    std::string diffuse_map_path = "";
    std::string specular_map_path = "";
    std::string normal_map_path = "";
    std::string height_map_path = "";
    float shininess{ 1.0f };
    //std::vector<Texture> textures;

    Shader* shader{ nullptr };

    void update_shader_binding();
    void set_specular_map(std::string map_path);
    void set_diffuse_map(std::string map_path);

};

#endif