#ifndef MyLight_hpp
#define MyLight_hpp
#include "MyCube.hpp"

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class MyLight : public MyCube {
public:
	MyLight(const glm::vec4& color);
    const Light& get_light() { return m_light; }
    void set_light(const Light& light) { m_light = light; }

protected:
	virtual void create_vbo() override;
	virtual void create_vao() override;
    Light m_light;
};

#endif