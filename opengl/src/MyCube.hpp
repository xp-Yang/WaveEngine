#ifndef MyCube_hpp
#define MyCube_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class MyCube {
public:
    MyCube();

    void generate_texture();
    
private:
    GLuint m_texuture_id;

};

#endif