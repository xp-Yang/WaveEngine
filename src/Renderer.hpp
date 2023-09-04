#ifndef Renderer_hpp
#define Renderer_hpp
#include "Shader.hpp"
#include "Mesh.hpp"
#include "View.hpp"

class Renderer {
public:
    Renderer() = default;
    void drawIndex(const Shader& shader, GLuint vao_id, size_t indices_count) const;
    void drawTriangle(const Shader& shader, GLuint vao_id, size_t array_count) const;
    void render(const View& view);
    void render_ecs(const View& view);
    void render_picking_border();
    void render_normal(const View& view);
};

#endif