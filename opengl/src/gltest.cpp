#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "myshader.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    switch (key) {
    case GLFW_KEY_UP:

        break;
    case GLFW_KEY_LEFT:
        break;
    case GLFW_KEY_RIGHT:
        break;
    case GLFW_KEY_DOWN:
        break;
    case GLFW_KEY_1:
        break;
    case GLFW_KEY_2:
        break;
    case GLFW_KEY_3:
        break;
    case GLFW_KEY_4:
        break;
    case GLFW_KEY_5:
        break;
    default:
        break;
    }
}

GLFWwindow* create_window(int size_x, int size_y) {
    //glfwInit��������ʼ��GLFW��glfwWindowHint����������GLFW
    glfwInit();
    //3.3�汾�Դ�����ϸ��ˣ����磺�����Լ�ָ�����Ƹ�glBindBuffer(),����Ҫʹ��glGenBuffers�����Ƿ��ص����ơ�
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3,����GLFW���ڴ���OpenGL������ʱ�����ʵ��ĵ���
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//��ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//���Ҳ������û��������ڵĴ�С

    //����һ�����ڶ���
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    //��ʼ��GLAD��ʹ����Թ���OpenGL����ָ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}

void set_view_port(int width, int height) {
    // ����Viewport
    // ���Ķ�������������塣
    // һ�������������������涨����Ӿ����еľ��ｫ����Ƶ�һ��ʲô�ߴ�Ļ���֮�ϣ�
    // ����һ�������ʾ������ƺõ�ͼ�񽫻ᱻ��ʾ����Ļ��ʲô����
    // �����ӿڱ任(Viewport Transform),��׼���豸�����任Ϊ��Ļ�ռ����ꡣ���õ���Ļ�ռ������ֻᱻ�任ΪƬ�����뵽Ƭ����ɫ���С�
    glViewport(0, 0, width, height);
}

void init_shaders() {
    MyShader shader("../shader/vshader.vs", "../shader/fshader.fs");

    //const GLchar* vertexShaderSource2 = "#version 330 core\n"
    //    "layout (location = 0) in vec3 position;\n"
    //    "layout (location = 1) in vec3 position2;\n"
    //    "layout (location = 2) in vec3 position3;\n"
    //    "void main()\n"
    //    "{\n"
    //    "gl_Position = vec4(position2.x, position2.y, position2.z, 1.0);\n"
    //    "}\0";
    //const GLchar* fragmentShaderSource2 = "#version 330 core\n"
    //    "uniform vec4 vari_color;\n"
    //    "out vec4 color\n"
    //    "void main()\n"
    //    "{\n"
    //    //"color = vari_color;\n"
    //    "color = vec4(0.0f, 1.0f, 1.0f, 1.0f);\n"
    //    "}\n\0";
}

int main()
{
    GLFWwindow* window = create_window(1080, 720);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(720, 720);

    MyShader shader("D:/Code/opengl/shader/vshader.vs", "D:/Code/opengl/shader/fshader.fs");

    //����VAO
    //VAO��һ�����������ж����������Ե�״̬��ϣ����洢�˶������ݵĸ�ʽ�Լ��������������VBO��������á�
    //VAO����û�д洢���������������ݣ���Щ��Ϣ�Ǵ洢��VBO�еģ�VAO�൱���ǶԺܶ��VBO�����ã���һЩVBO�����һ����Ϊһ������ͳһ����
    GLuint VAO1;//һ��id��vertext array object ���
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    GLfloat cubeVertex[] =
    {
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     1.0f, 0.0f, 1.0f, 1.0f, // 0
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     0.0f, 1.0f, 1.0f, 1.0f, // 1
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 0.0f, 1.0f, // 2
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 0.5f, 1.0f, 1.0f, // 3
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f,     0.5f, 1.0f, 1.0f, 1.0f, // 4
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 0.5f, 1.0f, // 5
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f, // 6
         0.5f, -0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f, // 7
    };
    GLuint cube_vbo;
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLubyte cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0
        4, 5, 6, 4, 6, 7, // Quad 1
        5, 3, 2, 5, 2, 6, // Quad 2
        4, 7, 1, 4, 1, 0, // Quad 3
        7, 6, 2, 7, 2, 1, // Quad 4
        4, 0, 3, 4, 3, 5  // Quad 5
    };
    GLuint cube_ibo;
    glGenBuffers(1, &cube_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);



    GLuint VAO2;//һ��id��vertext array object ���
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    GLfloat triVertex[] =
    {
        -1.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, -1.0f, 0.0f,
    };
    //�󶨵�һ�����㻺�����VBO1
    GLuint tri_vbo;//һ�����ƣ�vertex buffer object �����
	glGenBuffers(1, &tri_vbo);
    //��һ��������Ҫ���ɵĻ����������Ƶ��������ڶ����������洢����������Ƶ�����
    //�ú����᷵��n�������������Ƶ������(��ʵ���ϲ�û�д����κζ�������ֻ���ص�ǰδ�������������Ƶ������б�)
    //glGenBuffersʵ���ϸ�������Ҫ����ֻ����Ϊһ������ĺ���������һ��δʹ�õ�������
    //���ɵ������� GL ���Ϊ��ʹ�ã��������������ɡ������ַ�ʽ��ǵĶ������Ʋ��ᱻ�������÷�����������ͬ���͵����ƣ�ֱ��ͨ��ɾ����Щ�����ٴν�����Ϊδʹ��
	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //��һ������ָ�������������ͣ��ڶ�������ָ�������������ƣ�Ҳ����������glGenBuffers()�����ɵ�����
    //�ú������� 0 size �Ļ�����������Ĭ��״̬ GL_READ_WRITE �� GL_STATIC_DRAW
    //�ú������������󶨵�OpenGL�����Ļ����С�
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //�����Դ棬�������ɻ�����󣬸û�������������(glGenBuffers)������(glBindBuffer);
    //�����֮ǰ����Ķ������ݴ��䵽��ǰ�󶨵��Դ滺�����У����������ݴ���GPU��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //֪ͨOpenGL��ν�����Щ��������
    //��һ������ָ����������λ�ã��붥����ɫ����layout(location=0)��Ӧ��
    //�ڶ�������ָ���������Դ�С��
    //����������ָ���������͡�
    //���ĸ����������Ƿ�ϣ�����ݱ���׼����
    //����������ǲ�����Stride����ָ���������Ķ�������֮��ļ����
    //������������ʾ���ǵ�λ�������ڻ�������ʼλ�õ�ƫ������
	glEnableVertexAttribArray(0);



	//GLfloat test_vertices[] = {
	//	0.0f, 0.5f, 0.0f,
	//	0.0f, -0.5f, 0.0f,
	//	0.5f, 0.0f, 0.0f,
	//	0.5f, 0.5f, 0.0f,
	//};
 //   //�󶨵ڶ������㻺�����VBO2
 //   GLuint VBO2;
 //   glGenBuffers(1, &VBO2);
 //   glBindBuffer(GL_ARRAY_BUFFER, VBO2);
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(2);
 //   //���
 //   glBindBuffer(GL_ARRAY_BUFFER, 0);


    glfwSetKeyCallback(window, key_callback);
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {//��Ⱦ
        glfwPollEvents();//��鴥���¼�������������롢����ƶ��ȣ���Ȼ����ö�Ӧ�Ļص�����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.start_using();
        GLuint transformLoc = glGetUniformLocation(shader.get_id(), "transform");
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (GLfloat)glfwGetTime() / 5.0f, glm::vec3(0.5f, 0.3f, 0.5f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices), GL_UNSIGNED_BYTE, 0); // ʹ��_iboָ����36�����������ơ� 
        glBindVertexArray(0);

        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0);

        //glUseProgram(shaderProgram2);
        //GLfloat timeValue = glfwGetTime();
        //GLfloat vari_value1 = (sin(timeValue) / 2) + 0.5;
        //GLfloat vari_value2 = (cos(timeValue) / 2) + 0.5;
        //GLfloat vari_value3 = (sin(2 * timeValue) / 2) + 0.5;
        //GLint vertextColorLocation = glGetUniformLocation(shaderProgram2, "vari_color");
        //glUniform4f(vertextColorLocation, vari_value1, vari_value2, vari_value3, 1.0f);

        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    //glDeleteBuffers(1, &tri_vbo);
    //glDeleteBuffers(1, &VBO2);

    glfwTerminate();//����glfwTerminate�������ͷ�GLFW������ڴ�
    return 0;
}