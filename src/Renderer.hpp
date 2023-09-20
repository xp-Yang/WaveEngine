#ifndef Renderer_hpp
#define Renderer_hpp
#include "Shader.hpp"
#include "Mesh.hpp"
#include "View.hpp"

class Renderer {
public:
    static void drawIndex(const Shader& shader, GLuint vao_id, size_t indices_count);
    static void drawTriangle(const Shader& shader, GLuint vao_id, size_t array_count);
    static void render_picking_border();
    static void render_normal();
    static void render_shadow_map();
private:
    Renderer() = delete;
};

#endif