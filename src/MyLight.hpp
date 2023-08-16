#ifndef MyLight_hpp
#define MyLight_hpp
#include "Object.hpp"

//struct Light {
//    glm::vec3 position;
//    glm::vec3 ambient;
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//};

class MyLight : public Object {
public:
	MyLight();
    //const Light& get_light() { return m_light; }
    //void set_light(const Light& light) { m_light = light; }

protected:
    void init_mesh();
    void init_material();
    //Light m_light;
};

#endif