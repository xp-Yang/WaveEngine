#ifndef Material_hpp
#define Material_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

struct Material {
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    float diffuse_strength{ 1.0f };
    float specular_strength{ 1.0f };
    glm::vec3 diffuse{ 1.0f, 1.0f, 1.0f };
    glm::vec3 specular{ 1.0f, 1.0f, 1.0f };
    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    std::string diffuse_map_path = "";
    std::string specular_map_path = "";
    float shininess{ 1.0f };
};

#endif