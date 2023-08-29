#ifndef MyLight_hpp
#define MyLight_hpp
#include "Object.hpp"

class MyLight : public Object {
public:
	MyLight();
    const glm::mat4& get_light_space_matrix() const;

protected:
    void init_meshes();
    void init_materials() {};
};

#endif