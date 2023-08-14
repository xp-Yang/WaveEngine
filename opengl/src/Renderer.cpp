#include "Renderer.hpp"

void Renderer::draw(const Shader& shader, GLuint vao_id, DrawMode mode, int indices_count, int array_count)
{
    shader.start_using();

    glBindVertexArray(vao_id);

    if (mode == DrawMode::Arrays) {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else if (mode == DrawMode::Indices) {
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}