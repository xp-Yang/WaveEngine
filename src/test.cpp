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
    GLuint test_vao;//һ��id��vertext array object ���
    glGenVertexArrays(1, &test_vao);
    glBindVertexArray(test_vao);
    //   GLfloat triVertex[] =
    //   {
    //       -1.0f, -1.0f, 0.0f,
    //       -0.5f, -0.5f, 0.0f,
    //       -0.5f, -1.0f, 0.0f,
    //   };
    //   //�󶨵�һ�����㻺�����VBO1
    //   GLuint tri_vbo;//һ�����ƣ�vertex buffer object �����
    //   glGenBuffers(1, &tri_vbo);
    //   //��һ��������Ҫ���ɵĻ����������Ƶ��������ڶ����������洢����������Ƶ�����
    //   //�ú����᷵��n�������������Ƶ������(��ʵ���ϲ�û�д����κζ�������ֻ���ص�ǰδ�������������Ƶ������б�)
    //   //glGenBuffersʵ���ϸ�������Ҫ����ֻ����Ϊһ������ĺ���������һ��δʹ�õ�������
    //   //���ɵ������� GL ���Ϊ��ʹ�ã��������������ɡ������ַ�ʽ��ǵĶ������Ʋ��ᱻ�������÷�����������ͬ���͵����ƣ�ֱ��ͨ��ɾ����Щ�����ٴν�����Ϊδʹ��
    //   glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //   //��һ������ָ�������������ͣ��ڶ�������ָ�������������ƣ�Ҳ����������glGenBuffers()�����ɵ�����
    //   //�ú������� 0 size �Ļ�����������Ĭ��״̬ GL_READ_WRITE �� GL_STATIC_DRAW
    //   //�ú������������󶨵�OpenGL�����Ļ����С�
    //   glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //   //�����Դ棬ʹ����������������ݣ��û�������������(glGenBuffers)������(glBindBuffer);
    //   //�����֮ǰ����Ķ������ݴ��䵽��ǰ�󶨵��Դ滺�����У����������ݴ���GPU��
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //   //֪ͨOpenGL��ν�����Щ��������
    //   //��һ������ָ����������λ�ã��붥����ɫ����layout(location=0)��Ӧ��
    //   //�ڶ�������ָ���������Դ�С��
    //   //����������ָ���������͡�
    //   //���ĸ����������Ƿ�ϣ�����ݱ���׼����
    //   //����������ǲ�����Stride����ָ���������Ķ�������֮��ļ����
    //   //������������ʾ���ǵ�λ�������ڻ�������ʼλ�õ�ƫ������
    //   glEnableVertexAttribArray(0);
    return test_vao;
}


static glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(0.6f,  0.3f, -0.4f),
    glm::vec3(-0.3f, -0.2f, -0.3f),
};