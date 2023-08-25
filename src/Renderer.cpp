#include "Renderer.hpp"

void Renderer::draw(const Shader& shader, GLuint vao_id, DrawMode mode, int indices_count, int array_count) const
{
    shader.start_using();

    glBindVertexArray(vao_id);

    if (mode == DrawMode::Arrays) {
        glDrawArrays(GL_TRIANGLES, 0, array_count);
        //glDrawArrays(GL_LINE_STRIP, 0, array_count);
    }
    else if (mode == DrawMode::Indices) {
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}