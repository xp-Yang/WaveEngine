#ifndef MyCube_hpp
#define MyCube_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Object.hpp"
#include "MyMesh.hpp"
#include "Material.hpp"

class MyCube : public Object{
public:
    MyCube();
    MyCube(const Material& material);
    ~MyCube();

protected:
    void init_mesh();
    void init_material();
};

#endif