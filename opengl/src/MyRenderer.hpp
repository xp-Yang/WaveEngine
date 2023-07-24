#ifndef MyRenderer_hpp
#define MyRenderer_hpp
#include "MyShader.hpp"
#include <GLFW/glfw3.h>

enum class DrawMode {
    Indices,
    Arrays
};
class MyRenderer {
public:
    MyRenderer();
    void draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id, DrawMode mode, int indices_count = 0, int array_count = 0);

};

#endif