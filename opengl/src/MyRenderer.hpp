#ifndef MyRenderer_hpp
#define MyRenderer_hpp
#include "MyShader.hpp"
#include <GLFW/glfw3.h>

class MyRenderer {
public:
    MyRenderer();
    void draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id, int indices_count);

};

#endif