#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "myshader.hpp"
#include "stb_image.h"
#include "MyCube.hpp"

#define WINDOW_WIDTH 1080.0f
#define WINDOW_HEIGHT 720.0f
#define STEP 0.1f

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -15 * STEP));
glm::mat4 project = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
//���ĵ�һ������������fov��ֵ�������Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
//�ڶ������������˿�߱ȣ����ӿڵĿ���Ը����á�
//�����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ�档����ͨ�����ý�����Ϊ0.1f����Զ������Ϊ100.0f�������ڽ�ƽ���Զƽ�����Ҵ���ƽ��ͷ���ڵĶ��㶼�ᱻ��Ⱦ��

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    switch (key) {
    case GLFW_KEY_UP:
        view = glm::translate(view, glm::vec3(0.0f, STEP, 0.0f));
        break;
    case GLFW_KEY_LEFT:
        view = glm::translate(view, glm::vec3(-STEP, 0.0f, 0.0f));
        break;
    case GLFW_KEY_RIGHT:
        view = glm::translate(view, glm::vec3(STEP, 0.0f, 0.0f));
        break;
    case GLFW_KEY_DOWN:
        view = glm::translate(view, glm::vec3(0.0f, -STEP, 0.0f));
        break;
    case GLFW_KEY_HOME:
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, STEP));
        break;
    case GLFW_KEY_END:
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -STEP));
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

    std::cout << key << std::endl;
    std::cout << view[0].x << " " << view[1].x << " " << view[2].x << " " << view[3].x << " " << std::endl;
    std::cout << view[0].y << " " << view[1].y << " " << view[2].y << " " << view[3].y << " " << std::endl;
    std::cout << view[0].z << " " << view[1].z << " " << view[2].z << " " << view[3].z << " " << std::endl;
    std::cout << view[0].w << " " << view[1].w << " " << view[2].w << " " << view[3].w << " " << std::endl;
    std::cout << std::endl;
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
    GLFWwindow* window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(WINDOW_HEIGHT, WINDOW_HEIGHT);

    MyShader shader("../shader/vshader.vs", "../shader/fshader.fs");

    MyCube cube("../images/desert.jpg");


 //   GLuint VAO2;//һ��id��vertext array object ���
 //   glGenVertexArrays(1, &VAO2);
 //   glBindVertexArray(VAO2);
 //   GLfloat triVertex[] =
 //   {
 //       -1.0f, -1.0f, 0.0f,
 //       -0.5f, -0.5f, 0.0f,
 //       -0.5f, -1.0f, 0.0f,
 //   };
 //   //�󶨵�һ�����㻺�����VBO1
 //   GLuint tri_vbo;//һ�����ƣ�vertex buffer object �����
	//glGenBuffers(1, &tri_vbo);
 //   //��һ��������Ҫ���ɵĻ����������Ƶ��������ڶ����������洢����������Ƶ�����
 //   //�ú����᷵��n�������������Ƶ������(��ʵ���ϲ�û�д����κζ�������ֻ���ص�ǰδ�������������Ƶ������б�)
 //   //glGenBuffersʵ���ϸ�������Ҫ����ֻ����Ϊһ������ĺ���������һ��δʹ�õ�������
 //   //���ɵ������� GL ���Ϊ��ʹ�ã��������������ɡ������ַ�ʽ��ǵĶ������Ʋ��ᱻ�������÷�����������ͬ���͵����ƣ�ֱ��ͨ��ɾ����Щ�����ٴν�����Ϊδʹ��
	//glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
 //   //��һ������ָ�������������ͣ��ڶ�������ָ�������������ƣ�Ҳ����������glGenBuffers()�����ɵ�����
 //   //�ú������� 0 size �Ļ�����������Ĭ��״̬ GL_READ_WRITE �� GL_STATIC_DRAW
 //   //�ú������������󶨵�OpenGL�����Ļ����С�
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
 //   //�����Դ棬�������ɻ�����󣬸û�������������(glGenBuffers)������(glBindBuffer);
 //   //�����֮ǰ����Ķ������ݴ��䵽��ǰ�󶨵��Դ滺�����У����������ݴ���GPU��
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
 //   //֪ͨOpenGL��ν�����Щ��������
 //   //��һ������ָ����������λ�ã��붥����ɫ����layout(location=0)��Ӧ��
 //   //�ڶ�������ָ���������Դ�С��
 //   //����������ָ���������͡�
 //   //���ĸ����������Ƿ�ϣ�����ݱ���׼����
 //   //����������ǲ�����Stride����ָ���������Ķ�������֮��ļ����
 //   //������������ʾ���ǵ�λ�������ڻ�������ʼλ�õ�ƫ������
	//glEnableVertexAttribArray(0);


    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
    };

    glfwSetKeyCallback(window, key_callback);
    glEnable(GL_DEPTH_TEST);
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//��鴥���¼�������������롢����ƶ��ȣ���Ȼ����ö�Ӧ�Ļص�����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

        shader.start_using();

        GLfloat timeValue = glfwGetTime();
        GLfloat normalization_time = (sin(timeValue) / 3) + 0.6;
        shader.setFloat("time_var", normalization_time);

        model = glm::rotate(glm::mat4(1.0f), normalization_time * 20.0f, glm::vec3(0.5f, 0.3f, 0.5f));

        shader.setMatrix("model", 1, model);
        shader.setMatrix("view", 1, view);
        shader.setMatrix("projection", 1, project);

        glBindVertexArray(cube.get_vao_id());
        for (int i = 0; i < 3; i++) {
            model = glm::translate(model, cubePositions[i]);
            glDrawElements(GL_TRIANGLES, /*sizeof(cubeIndices)*/36, GL_UNSIGNED_BYTE, 0); // ʹ��_iboָ����36�����������ơ� 
        }
        glBindVertexArray(0);

        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0);

        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }

    //glDeleteVertexArrays(1, &VAO2);

    glfwTerminate();//����glfwTerminate�������ͷ�GLFW������ڴ�
    return 0;
}