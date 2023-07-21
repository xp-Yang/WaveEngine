#include "MyRenderer.hpp"

MyRenderer::MyRenderer() {}

void MyRenderer::draw(GLFWwindow* window, const MyShader& shader, GLuint vao_id)
{
    glBindVertexArray(vao_id);

    glDrawElements(GL_TRIANGLES, /*cube.get_elements_count()*/36, GL_UNSIGNED_BYTE, 0); // 使用cube.ibo指定的36个索引来绘制。 

    glBindVertexArray(0);
}