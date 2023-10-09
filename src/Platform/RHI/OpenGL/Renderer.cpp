#include "Renderer.hpp"
#include <glad/glad.h>
#include "Shader.hpp"

void Renderer::drawIndex(const Shader& shader, unsigned int vao_id, size_t indices_count)
{
    shader.start_using();
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, (int)indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawTriangle(const Shader& shader, unsigned int vao_id, size_t array_count)
{
    shader.start_using();
    glBindVertexArray(vao_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)array_count);
    glBindVertexArray(0);
}