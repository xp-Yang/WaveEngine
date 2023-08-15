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

#include "MyCube.hpp"
#include "MyLight.hpp"
#include "MyGround.hpp"
#include "MyModel.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "stb_image.h"

Camera camera({ 0.0f, 5.0f, 5.0f }, -45.0f, 0.0f, 0.0f);

static float delta_time = 0.0f; // 当前帧与上一帧的时间差
static int mouse_left_status;
static int mouse_right_status;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    camera.key_process(key, delta_time);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == 0) {
        if (action == GLFW_PRESS)
            mouse_left_status = 1;
        if (action == GLFW_RELEASE)
            mouse_left_status = 0;
    }
    if (button == 1) {
        if (action == GLFW_PRESS)
            mouse_right_status = 1;
        if (action == GLFW_RELEASE)
            mouse_right_status = 0;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{ 
    static int last_left_mouse_status = mouse_left_status;
    static int last_right_mouse_status = mouse_right_status;

    static float last_pos_x = WINDOW_WIDTH / 2;
    static float last_pos_y = WINDOW_HEIGHT / 2;
    if (last_left_mouse_status != mouse_left_status) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_left_mouse_status = mouse_left_status;
    }
    if (mouse_left_status) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y); // 注意这里是相反的，因为glfwSetCursorPosCallback返回给mouse_callback函数的 (x,y) 是鼠标相对于窗口左上角的位置，所以需要将 (ypos - lastY) 取反
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 0);
    }

    if (last_right_mouse_status != mouse_right_status) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_right_mouse_status = mouse_right_status;
    }
    if (mouse_right_status) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y);
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 1);
    }
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
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "RenderEngine", nullptr, nullptr);
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

void init_cube() {
    MyCube cube("resource/images/desert.jpg", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Shader cube_shader("resource/shader/cube.vs", "resource/shader/cube.fs");
    Material cube_material{};
}

void render_imgui() {

    //auto& cube_material = cube.get_material();
    //auto& ground_material = ground.get_material();
    //auto& cube_mat = cube.get_model_matrix();
    //glm::vec3 cube_offset = cube_mat[3];
    //static float ambient_strength = 0.3f;

    //ImGui::Begin("Controller");

    //ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
    //ImGui::SliderFloat("cube shininess", &cube_material.shininess, 0, 1.5f);

    //ImGui::SliderFloat("ground diffuse strength", &ground_material.diffuse_strength, 0, 1);
    //ImGui::SliderFloat("ground specular strength", &ground_material.specular_strength, 0, 1);
    //ImGui::SliderFloat("ground shininess", &ground_material.shininess, 0, 256);

    //ImGui::PushItemWidth(85.0f);
    //ImGui::SliderFloat("##cube x", &cube_offset.x, -10.0f, 10.0f);
    //ImGui::SameLine();
    //ImGui::SliderFloat("##cube y", &cube_offset.y, -10.0f, 10.0f);
    //ImGui::SameLine();
    //ImGui::SliderFloat("cube xyz", &cube_offset.z, -10.0f, 10.0f);
    //ImGui::PopItemWidth();
    ////ImGui::ColorEdit3("light color", (float*)&light_color);
    ////ImGui::ColorEdit3("cube color", (float*)&cube_color);
    ////ImGui::ColorEdit3("ground color", (float*)&ground_color);

    ////if (ImGui::Checkbox("stop rotate", &stop_rotate));

    //// log
    //{
    //    ImGui::NewLine();
    //    ImGui::Text("light matrix:");
    //    std::string test_light = matrix_log(light.get_model_matrix());
    //    ImGui::Text(test_light.c_str());

    //    ImGui::NewLine();
    //    ImGui::Text("cube matrix:");
    //    std::string test_cube = matrix_log(cube.get_model_matrix());
    //    ImGui::Text(test_cube.c_str());

    //    //ImGui::NewLine();
    //    //ImGui::Text("ground matrix:");
    //    //std::string test_ground = matrix_log(ground.get_model_matrix());
    //    //ImGui::Text(test_ground.c_str());

    //    ImGui::NewLine();
    //    ImGui::Text("view matrix:");
    //    std::string test_view = matrix_log(camera.get_view());
    //    ImGui::Text(test_view.c_str());

    //    ImGui::NewLine();
    //    ImGui::Text("camera position:");
    //    std::string test_camera_pos = vec3_log(camera.get_position());
    //    ImGui::Text(test_camera_pos.c_str());

    //    ImGui::NewLine();
    //    ImGui::Text("camera direction:");
    //    std::string test_camera_dir = vec3_log(camera.get_direction().dir);
    //    ImGui::Text(test_camera_dir.c_str());

    //    ImGuiIO& io = ImGui::GetIO();
    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    //}

    //ImGui::End();
}

void start_loop(GLFWwindow* window) {
    Shader cube_shader("resource/shader/cube.vs", "resource/shader/cube.fs");
    Shader ground_shader("resource/shader/ground.vs", "resource/shader/ground.fs");
    Shader light_shader("resource/shader/light.vs", "resource/shader/light.fs");
    Shader model_shader("resource/shader/model.vs", "resource/shader/model.fs");
    Model model("resource/model/nanosuit/nanosuit.obj");
    MyGround ground(glm::vec4(255.0f / 255.0f, 255.0f / 255.0f, 216.0f / 255.0f, 1.0f));
    MyCube cube("resource/images/desert.jpg", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    MyLight light(glm::vec4(1.0f));
    Renderer renderer;

    static float ambient_strength = 0.3f;
    static float cube_shininess = 1.0f;
    static float ground_diffuse = 0.5f;
    static float ground_spec = 0.5f;
    static float ground_shininess = 1.0f;
    static glm::vec3 light_color = { 1.0f, 1.0f, 1.0f };

    Material cube_material{};

    glEnable(GL_DEPTH_TEST);

    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数

        glm::vec4 light_bg = glm::vec4(light_color * ambient_strength * 0.8f, 1.0f);
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
        //static ImVec4 light_color = { light_color.x, light_color.y, light_color.z, 1.0f };
        light_shader.setFloat3("color", light_color);
        renderer.draw(light_shader, light.get_vao_id(), DrawMode::Indices, light.get_elements_count());


        // render cube
        cube_shader.start_using();
        cube_shader.setMatrix("view", 1, camera.get_view());
        cube_shader.setFloat3("viewpos", camera.get_position());
        cube_shader.setMatrix("projection", 1, camera.get_projection());
        cube_shader.setFloat("material.ambient", ambient_strength);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.get_texture_id());
        cube_shader.setTexture("material.diffuse_map", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.get_specular_map_id());
        cube_shader.setTexture("material.specular_map", 1);

        cube_shader.setFloat3("light.color", light_color);
        cube_shader.setFloat3("light.position", light.get_model_matrix()[3]);
        cube_shader.setFloat("material.shininess", cube_shininess);
        static bool stop_rotate = true;
        static float time_value = 0.0f;
        if (stop_rotate) {
            ;
        }
        if (!stop_rotate) {
            time_value = normalization_time;
        }
        static float cube_translate_x = 0.0f;
        static float cube_translate_y = 1.0f;
        static float cube_translate_z = 0.0f;
        auto translate = glm::translate(glm::mat4(1.0f), { cube_translate_x, cube_translate_y, cube_translate_z });
        auto rotate = glm::rotate(glm::mat4(1.0f), time_value * 20.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        cube.set_model_matrix(translate * rotate * glm::mat4(1.0f));
        cube_shader.setMatrix("model", 1, cube.get_model_matrix());
        //static ImVec4 cube_color = { cube.get_color().x, cube.get_color().y, cube.get_color().z, 1.0f };
        //cube.set_color({ cube_color.x, cube_color.y, cube_color.z });
        cube_shader.setFloat3("color", glm::vec3(1.0f));
        renderer.draw(cube_shader, cube.get_vao_id(), DrawMode::Indices, cube.get_elements_count());

        // render model
        model_shader.start_using();
        model_shader.setMatrix("view", 1, camera.get_view());
        model_shader.setMatrix("projection", 1, camera.get_projection());
        auto nanosuit_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));
        auto nanosuit_translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        model_shader.setMatrix("model", 1, nanosuit_translate * nanosuit_scale * cube.get_model_matrix());
        model.draw(model_shader);

        // render ground
        ground_shader.start_using();
        ground_shader.setMatrix("view", 1, camera.get_view());
        ground_shader.setFloat3("viewpos", camera.get_position());
        ground_shader.setMatrix("projection", 1, camera.get_projection());
        ground_shader.setFloat("material.ambient", ambient_strength);
        ground_shader.setFloat("material.diffuse", ground_diffuse);
        ground_shader.setFloat("material.specular", ground_spec);
        ground_shader.setFloat("material.shininess", ground_shininess);
        ground_shader.setFloat3("light.color", light_color);
        ground_shader.setFloat3("light.position", light.get_model_matrix()[3]);
        ground_shader.setMatrix("model", 1, ground.get_model_matrix());
        //static ImVec4 ground_color = { ground.get_color().x, ground.get_color().y, ground.get_color().z, 1.0f };
        //ground.set_color({ ground_color.x, ground_color.y, ground_color.z });
        ground_shader.setFloat3("color", glm::vec3(1.0f));
        renderer.draw(ground_shader, ground.get_vao_id(), DrawMode::Indices, ground.get_elements_count());

        render_imgui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并输出显示在屏幕上。
    }

}

int main()
{
    GLFWwindow* window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT);
    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS 模式


    // setup imgui
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


    start_loop(window);


    // shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}