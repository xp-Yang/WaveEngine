#include "OpenGLRenderer.hpp"

void OpenGLRenderer::drawIndexed(unsigned int vao_id, size_t indices_count)
{
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, (int)indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLRenderer::drawTriangle(unsigned int vao_id, size_t array_count)
{
    glBindVertexArray(vao_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)array_count);
    glBindVertexArray(0);
}
