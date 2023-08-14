#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint test_vao()
{
    GLuint test_vao;//一个id，vertext array object 句柄
    glGenVertexArrays(1, &test_vao);
    glBindVertexArray(test_vao);
    //   GLfloat triVertex[] =
    //   {
    //       -1.0f, -1.0f, 0.0f,
    //       -0.5f, -0.5f, 0.0f,
    //       -0.5f, -1.0f, 0.0f,
    //   };
    //   //绑定第一个顶点缓冲对象VBO1
    //   GLuint tri_vbo;//一个名称（vertex buffer object 句柄）
    //   glGenBuffers(1, &tri_vbo);
    //   //第一个参数是要生成的缓冲对象的名称的数量，第二个是用来存储缓冲对象名称的数组
    //   //该函数会返回n个缓冲对象的名称到数组里。(它实际上并没有创建任何东西。它只返回当前未用作缓冲区名称的整数列表。)
    //   //glGenBuffers实际上根本不需要，它只是作为一个方便的函数来给你一个未使用的整数。
    //   //生成的名称由 GL 标记为已使用，仅用于名称生成。以这种方式标记的对象名称不会被其他调用返回以生成相同类型的名称，直到通过删除这些名称再次将其标记为未使用
    //   glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //   //第一个参数指定缓冲对象的类型，第二个参数指定缓冲对象的名称，也就是我们在glGenBuffers()里生成的名称
    //   //该函数创建 0 size 的缓冲对象，其具有默认状态 GL_READ_WRITE 和 GL_STATIC_DRAW
    //   //该函数将缓冲对象绑定到OpenGL上下文环境中。
    //   glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //   //分配显存，使缓冲对象真正有数据，该缓冲对象具有名称(glGenBuffers)、类型(glBindBuffer);
    //   //它会把之前定义的顶点数据传输到当前绑定的显存缓冲区中，（顶点数据传入GPU）
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //   //通知OpenGL如何解释这些顶点数据
    //   //第一个参数指定顶点属性位置，与顶点着色器中layout(location=0)对应。
    //   //第二个参数指定顶点属性大小。
    //   //第三个参数指定数据类型。
    //   //第四个参数定义是否希望数据被标准化。
    //   //第五个参数是步长（Stride），指定在连续的顶点属性之间的间隔。
    //   //第六个参数表示我们的位置数据在缓冲区起始位置的偏移量。
    //   glEnableVertexAttribArray(0);
    return test_vao;
}


static glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(0.6f,  0.3f, -0.4f),
    glm::vec3(-0.3f, -0.2f, -0.3f),
};