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
#include "MyLight.hpp"
#include "MyCamera.hpp"
#include "MyRenderer.hpp"
#include "stb_image.h"

MyCamera camera({ 0.0f, 0.0f, 10.0f }, glm::vec3(0.0f));

static float delta_time = 0.0f; // 当前帧与上一帧的时间差

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
    float delta_y = -(ypos - last_ypos); // 注意这里是相反的，因为glfwSetCursorPosCallback返回给mouse_callback函数的 (x,y) 是鼠标相对于窗口左上角的位置，所以需要将 (ypos - lastY) 取反
    last_xpos = xpos;
    last_ypos = ypos;

    camera.mouse_process(delta_x, delta_y);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.mouse_scroll_process(yoffset);
}


GLFWwindow* create_window(int size_x, int size_y) {
    //glfwInit函数来初始化GLFW，glfwWindowHint函数来配置GLFW
    glfwInit();
    //3.3版本对代码更严格了：比如：不能自己指定名称给glBindBuffer(),而是要使用glGenBuffers给我们返回的名称。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3,这样GLFW会在创建OpenGL上下文时做出适当的调整
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//明确告诉GLFW我们使用的是核心模式(Core-profile)
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//并且不允许用户调整窗口的大小

    //创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    //初始化GLAD，使其可以管理OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}

void set_view_port(int width, int height) {
    // 设置Viewport
    // 它的定义具有两个意义。
    // 一个意义是它定义了上面定义的视景体中的景物将会绘制到一张什么尺寸的画布之上，
    // 另外一个意义表示这个绘制好的图像将会被显示在屏幕的什么区域。
    // 进行视口变换(Viewport Transform),标准化设备坐标会变换为屏幕空间坐标。所得的屏幕空间坐标又会被变换为片段输入到片段着色器中。
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

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(WINDOW_WIDTH, WINDOW_HEIGHT);

    MyShader cube_shader("resource/shader/cube.vs", "resource/shader/cube.fs");
    MyShader light_shader("resource/shader/light.vs", "resource/shader/light.fs");
    MyCube cube("resource/images/desert.jpg", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    MyLight light(glm::vec4(1.0f));
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

    static float ambient_strength = 0.3f;

    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数

        glm::vec4 light_bg = glm::vec4(light.get_color() * ambient_strength * 0.8f, 1.0f);
        glClearColor(light_bg.x, light_bg.y, light_bg.z, light_bg.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float last_time = 0.0f; // 上一帧的时间
        GLfloat curr_time = glfwGetTime();

        delta_time = curr_time - last_time;
        last_time = curr_time;

        float normalization_time = (sin(curr_time) / 3) + 0.6;
        
        // render light
            light_shader.start_using();
            light_shader.setMatrix("view", 1, camera.get_view());
            light_shader.setMatrix("projection", 1, camera.get_projection());

            light_shader.setMatrix("model", 1, light.get_model_matrix());
            static ImVec4 light_color = { light.get_color().x, light.get_color().y, light.get_color().z, 1.0f };
            light.set_color({ light_color.x, light_color.y, light_color.z });
            light_shader.setFloat3("color", light.get_color());
            renderer.draw(window, light_shader, light.get_vao_id(), light.get_elements_count());
        

        // render cube
            cube_shader.start_using();
            cube_shader.setMatrix("view", 1, camera.get_view());
            cube_shader.setMatrix("projection", 1, camera.get_projection());
            cube_shader.setFloat("ambient_strength", ambient_strength);
            cube_shader.setFloat3("light_color", light.get_color());
            cube_shader.setFloat3("light_pos", light.get_model_matrix()[3]);
            static bool stop_rotate = false;
            static float time_value = normalization_time;
            if (stop_rotate) {
                ;
            }
            if (!stop_rotate) {
                time_value = normalization_time;
            }
            static float cube_translate_x = 0.0f;
            auto translate = glm::translate(glm::mat4(1.0f), { cube_translate_x, 0.0f, 0.0f });
            auto rotate = glm::rotate(glm::mat4(1.0f), time_value * 20.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            cube.set_model_matrix(translate * rotate * glm::mat4(1.0f));
            cube_shader.setMatrix("model", 1, cube.get_model_matrix());
            static ImVec4 cube_color = { cube.get_color().x, cube.get_color().y, cube.get_color().z, 1.0f };
            cube.set_color({ cube_color.x, cube_color.y, cube_color.z });
            //shader.setFloat3("color", ambient_light * cube.get_color());
            cube_shader.setFloat3("color", cube.get_color());
            renderer.draw(window, cube_shader, cube.get_vao_id(), cube.get_elements_count());


        // imgui window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
            ImGui::SliderFloat("cube x", &cube_translate_x, -10.0f, 10.0f);
            ImGui::ColorEdit3("light color", (float*)&light_color);
            ImGui::ColorEdit3("cube color", (float*)&cube_color);

            if (ImGui::Checkbox("stop rotate", &stop_rotate));
            
            ImGui::NewLine();
            ImGui::Text("light matrix:");
            std::string test_light = matrix_log(light.get_model_matrix());
            ImGui::Text(test_light.c_str());

            ImGui::NewLine();
            ImGui::Text("cube matrix:");
            std::string test_cube = matrix_log(cube.get_model_matrix());
            ImGui::Text(test_cube.c_str());

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并输出显示在屏幕上。
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

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