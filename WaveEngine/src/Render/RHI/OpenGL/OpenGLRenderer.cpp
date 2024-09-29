#include "OpenGLRenderer.hpp"

void OpenGLRenderer::drawIndexed(unsigned int vao_id, size_t indices_count, int inst_amount)
{
    glBindVertexArray(vao_id);
    if (inst_amount == -1)
        glDrawElements(GL_TRIANGLES, (int)indices_count, GL_UNSIGNED_INT, 0);
    else
        glDrawElementsInstanced(GL_TRIANGLES, (int)indices_count, GL_UNSIGNED_INT, 0, inst_amount);
    glBindVertexArray(0);
}

void OpenGLRenderer::drawTriangle(unsigned int vao_id, size_t array_count)
{
    glBindVertexArray(vao_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)array_count);
    glBindVertexArray(0);
}
