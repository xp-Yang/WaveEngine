#ifndef Material_hpp
#define Material_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Material {
    glm::vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    unsigned int diffuse_map;
    unsigned int specular_map;
    float shininess;
};

#endif