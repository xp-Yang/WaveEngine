#ifndef Renderer_hpp
#define Renderer_hpp
#include "ResourceManager/Shader.hpp"
#include "ResourceManager/Mesh.hpp"

class Renderer {
public:
    static void drawIndex(const Shader& shader, unsigned int vao_id, size_t indices_count);
    static void drawTriangle(const Shader& shader, unsigned int vao_id, size_t array_count);
private:
    Renderer() = delete;
};

#endif