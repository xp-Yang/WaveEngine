#include "MyRenderer.hpp"

MyRenderer::MyRenderer() {}

void MyRenderer::draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id, int indices_count)
{
    shader.start_using();

    glBindVertexArray(vao_id);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, 0);

    glBindVertexArray(0);
}