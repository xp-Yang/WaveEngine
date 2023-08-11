#ifndef MyRenderer_hpp
#define MyRenderer_hpp
#include "MyShader.hpp"
#include "MyMesh.hpp"
#include <GLFW/glfw3.h>

enum class DrawMode {
    Indices,
    Arrays
};
class MyRenderer {
public:
    MyRenderer() = default;
    void draw(const MyShader& shader, GLuint vao_id, DrawMode mode, int indices_count = 0, int array_count = 0, std::vector<Texture> textures = {});

};

#endif