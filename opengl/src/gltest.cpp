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
#include "MyShader.hpp"
#include "MyCube.hpp"
#include "MyCamera.hpp"
#include "MyRenderer.hpp"
#include "stb_image.h"

#define WINDOW_WIDTH 1080.0f
#define WINDOW_HEIGHT 720.0f

MyCamera camera({ 0.0f, 0.5f, 2.0f }, glm::vec3(0.0f));

static float delta_time = 0.0f; // ��ǰ֡����һ֡��ʱ���

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    camera.key_process(key, delta_time);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{ 
    static float last_xpos = WINDOW_WIDTH / 2;
    static float last_ypos = WINDOW_HEIGHT / 2;

    static bool first_enter = true;
    if (first_enter)
    {
        last_xpos = xpos;
        last_ypos = ypos;
        first_enter = false;
        return;
    }

    float delta_x = xpos - last_xpos;
    float delta_y = -(ypos - last_ypos); // ע���������෴�ģ���ΪglfwSetCursorPosCallback���ظ�mouse_callback������ (x,y) ���������ڴ������Ͻǵ�λ�ã�������Ҫ�� (ypos - lastY) ȡ��
    last_xpos = xpos;
    last_ypos = ypos;

    camera.mouse_process(delta_x, delta_y);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.mouse_scroll_process(yoffset);
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

static glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(0.6f,  0.3f, -0.4f),
    glm::vec3(-0.3f, -0.2f, -0.3f),
};

int main()
{
    GLFWwindow* window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(WINDOW_WIDTH, WINDOW_HEIGHT);

    MyShader shader("resource/shader/vshader.vs", "resource/shader/fshader.fs");
    MyCube cube("resource/images/desert.jpg");
    MyRenderer renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//��鴥���¼�������������롢����ƶ��ȣ���Ȼ����ö�Ӧ�Ļص�����

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float last_time = 0.0f; // ��һ֡��ʱ��
        GLfloat curr_time = glfwGetTime();

        delta_time = curr_time - last_time;
        last_time = curr_time;

        GLfloat normalization_time = (sin(curr_time) / 3) + 0.6;

        shader.start_using();
        shader.setFloat("time_var", normalization_time);
        shader.setMatrix("view", 1, camera.get_view());
        shader.setMatrix("projection", 1, camera.get_projection());

        auto rotate = glm::rotate(glm::mat4(1.0f), normalization_time * 20.0f, glm::vec3(0.5f, 0.3f, 0.5f));
        auto translate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f});
        cube.set_model_matrix(rotate * translate * glm::mat4(1.0f));
        shader.setMatrix("model", 1, cube.get_model_matrix());
        renderer.draw(window, shader, cube.get_vao_id(), cube.get_elements_count());

        // Show another simple window.
        {
            ImGui::Begin("Another Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                ;
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();//����glfwTerminate�������ͷ�GLFW������ڴ�
    return 0;
}

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
    //   //�����Դ棬�������ɻ�����󣬸û�������������(glGenBuffers)������(glBindBuffer);
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