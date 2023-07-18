#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "myshader.hpp"
#include "stb_image.h"

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
    case GLFW_KEY_SPACE:

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

int main()
{
    GLFWwindow* window = create_window(1080, 720);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(720, 720);

    MyShader shader("../shader/vshader.vs", "../shader/fshader.fs");

    //����VAO
    //VAO��һ�����������ж����������Ե�״̬��ϣ����洢�˶������ݵĸ�ʽ�Լ��������������VBO��������á�
    //VAO����û�д洢���������������ݣ���Щ��Ϣ�Ǵ洢��VBO�еģ�VAO�൱���ǶԺܶ��VBO�����ã���һЩVBO�����һ����Ϊһ������ͳһ����
    GLuint VAO1;//һ��id��vertext array object ���
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    GLfloat cubeVertex[] =
    {
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     1.0f, 0.0f, 1.0f, 1.0f, // 0 ����
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     0.0f, 1.0f, 1.0f, 1.0f, // 1 ����
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 0.0f, 1.0f, // 2 ����
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 0.5f, 1.0f, 1.0f, // 3 ����
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

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

    int width, height, nrChannels;
    unsigned char* data = stbi_load("../images/desert.jpg", &width, &height, &nrChannels, 0);
    GLuint cube_tex;
    glGenTextures(1, &cube_tex);
    glBindTexture(GL_TEXTURE_2D, cube_tex);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
    //�ڶ�������Ϊ����ָ���༶��Զ����ļ��������ϣ�������ֶ�����ÿ���༶��Զ����ļ���Ļ�������������0��Ҳ���ǻ�������
    //��������������OpenGL����ϣ����������Ϊ���ָ�ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
    //���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�����֮ǰ����ͼ���ʱ�򴢴������ǣ���������ʹ�ö�Ӧ�ı�����
    //�¸�����Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
    //���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�����ʹ��RGBֵ�������ͼ�񣬲������Ǵ���Ϊchar(byte)���飬���ǽ��ᴫ���Ӧֵ��
    //���һ��������������ͼ�����ݡ�
    glGenerateMipmap(GL_TEXTURE_2D);
    //�ͷ�ͼ����ڴ�
    stbi_image_free(data);


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



    glfwSetKeyCallback(window, key_callback);
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {//��Ⱦ
        glfwPollEvents();//��鴥���¼�������������롢����ƶ��ȣ���Ȼ����ö�Ӧ�Ļص�����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.start_using();
        GLfloat timeValue = glfwGetTime();
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, timeValue / 5.0f, glm::vec3(0.5f, 0.3f, 0.5f));
        shader.setMatrix("transform", 1, trans);

        GLfloat normalization_time = (sin(timeValue) / 3) + 0.6;
        shader.setFloat("time_var", normalization_time);

        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices), GL_UNSIGNED_BYTE, 0); // ʹ��_iboָ����36�����������ơ� 
        glBindVertexArray(0);

        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0);


        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    //glDeleteBuffers(1, &tri_vbo);
    //glDeleteBuffers(1, &VBO2);

    glfwTerminate();//����glfwTerminate�������ͷ�GLFW������ڴ�
    return 0;
}