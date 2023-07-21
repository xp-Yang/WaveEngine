#ifndef MyRenderer_hpp
#define MyRenderer_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyShader.hpp"

class MyRenderer {
public:
    MyRenderer();
    void draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id);

};

#endif