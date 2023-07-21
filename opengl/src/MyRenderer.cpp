#include "MyRenderer.hpp"

MyRenderer::MyRenderer() {}

void MyRenderer::draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id)
{
    glBindVertexArray(vao_id);

    glDrawElements(GL_TRIANGLES, /*cube.get_elements_count()*/36, GL_UNSIGNED_BYTE, 0); // ʹ��cube.iboָ����36�����������ơ� 

    glBindVertexArray(0);
}