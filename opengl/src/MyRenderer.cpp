#include "MyRenderer.hpp"

MyRenderer::MyRenderer() {}

void MyRenderer::draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id, int indices_count)
{
    shader.start_using();

    glBindVertexArray(vao_id);

    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_BYTE, 0); // ʹ��cube.iboָ����36�����������ơ� 

    glBindVertexArray(0);
}