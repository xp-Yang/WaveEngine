#ifndef Material_hpp
#define Material_hpp

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;
struct Material {
    //float diffuse_strength{ 1.0f };
    //float specular_strength{ 1.0f };
    float ambient_strength{ 0.1f };
    unsigned int diffuse_map{ 0 };
    unsigned int specular_map{ 0 };
    unsigned int normal_map{ 0 };
    unsigned int height_map{ 0 };
        //debug
        std::string diffuse_map_path = "";
        std::string specular_map_path = "";
        std::string normal_map_path = "";
        std::string height_map_path = "";
    float shininess{ 128.0f };

    // TODO：Shader 设计考量，是共享其他shader资源，还是自己拥有
    Shader* shader{ nullptr };

    void update_shader_binding() const;
    //void set_shader();
    void set_specular_map(std::string map_path);
    void set_diffuse_map(std::string map_path);
};

#endif