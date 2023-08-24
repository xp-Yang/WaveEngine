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

#include "Object.hpp"
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

#define PI (3.14159)

Scene scene;

static float delta_time = 0.0f; // 当前帧与上一帧的时间差
static int mouse_left_status;
static int mouse_right_status;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    auto camera = scene.camera();
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    camera->key_process(key, delta_time);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }
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
    auto camera = scene.camera();
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }
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
        camera->mouse_process(delta_x, delta_y, 0);
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
        camera->mouse_process(delta_x, delta_y, 1);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto camera = scene.camera();
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }
    camera->mouse_scroll_process(yoffset);
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

void init_scene() {
    MyLight* light = new MyLight();
    Material light_material;
    light_material.shader = new Shader("resource/shader/light.vs", "resource/shader/light.fs");
    light_material.color = glm::vec3(1.0f);
    light->set_material(light_material);

    MyCube* cube = new MyCube();
    Material cube_material;
    cube_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    cube_material.set_diffuse_map("resource/images/desert.jpg");
    cube_material.set_specular_map("resource/images/cube_specular.png");
    cube->set_material(cube_material);

    MyGround* ground = new MyGround();
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.color = glm::vec3(1.0f);
    ground_material.diffuse_strength = 0.5f;
    ground_material.specular_strength = 0.5f;
    ground->set_material(ground_material);

    Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
    Shader* model_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    for (auto& material : nanosuit->m_materials) {
        material.shader = model_shader;
    }

    Model* yoko = new Model("resource/model/yoko/008.obj");
    for (auto& material : yoko->m_materials) {
        material.shader = model_shader;
    }

    Camera* camera = new Camera({ 0.0f, 10.0f, 10.0f }, -35.0f/*-atan(1.0f / 10.0f) * 180.0f / PI*/, 0.0f, 0.0f);

    scene.append_object(light);
    scene.append_object(cube);
    scene.append_object(ground);
    scene.append_object(nanosuit);
    scene.append_object(yoko);
    scene.set_camera(camera);
}

static bool pixel_style = false;
static bool stop_rotate = false;
static float ambient_strength = 0.1f;
static float magnitude = 0.0f;
unsigned int tex_depth_buffer;

void render_scene(Shader* depth_shader) {
    Camera* camera = scene.camera();
    auto& light = *scene.object(0);
    auto& cube = *scene.object(1);
    auto& ground = *scene.object(2);
    Model& nanosuit = *static_cast<Model*>(scene.object(3));
    Model& yoko = *static_cast<Model*>(scene.object(4));

    Shader* light_shader = light.material().shader;
    Shader* model_shader = nanosuit.m_materials[0].shader;

    Renderer renderer;

    static float last_time = 0.0f; // 上一帧的时间
    GLfloat curr_time = glfwGetTime();

    delta_time = curr_time - last_time;
    last_time = curr_time;

    glm::mat4 lightProjection = glm::ortho(-10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -5.0f, 15.0f, 0.1f, 100.0f);
    //lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 light_pos = light.get_model_matrix()[3];
    glm::mat4 lightView = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    if (depth_shader) {
        depth_shader->start_using();
        depth_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
    }
    light_shader->start_using();
    light_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
    model_shader->start_using();
    model_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);

    model_shader->start_using();
    model_shader->setMatrix("view", 1, camera->get_view());
    model_shader->setMatrix("projection", 1, camera->get_projection());
    model_shader->setFloat("material.ambient", ambient_strength);
    model_shader->setFloat3("viewpos", camera->get_position());
    model_shader->setFloat3("light.color", light.get_material().color);
    model_shader->setFloat3("light.position", light.get_model_matrix()[3]);

    // render light
    {
        light_shader->start_using();
        static float time_value = 0.0f;
        if (stop_rotate) {
            ;
        }
        if (!stop_rotate) {
            time_value = curr_time;
        }
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 displacement(1.0f);
        displacement[3].x = 7.0f;
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value * 3, glm::vec3(0.0f, 1.0f, 0.0f));
        auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 5.0f, 0.0f });
        light.set_model_matrix(translate * rotate * displacement * scale);
        light_shader->setMatrix("model", 1, light.get_model_matrix());
        light_shader->setMatrix("view", 1, camera->get_view());
        light_shader->setMatrix("projection", 1, camera->get_projection());
        light_shader->setFloat3("color", light.get_material().color);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, light.get_model_matrix());
            renderer.draw(*depth_shader, light.get_mesh().get_VAO(), DrawMode::Indices, light.get_mesh().get_indices_count());
        }else
            renderer.draw(*light_shader, light.get_mesh().get_VAO(), DrawMode::Indices, light.get_mesh().get_indices_count());
    }

    // render ground
    {
        model_shader->start_using();
        model_shader->setMatrix("model", 1, ground.get_model_matrix());
        model_shader->setFloat3("color", glm::vec3(1.0f));
        model_shader->setFloat("material.shininess", ground.get_material().shininess);
        // TODO 设置重复铺满
        static unsigned int default_map = Mesh::generate_texture_from_file("resource/images/default_white_map.png", false);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, default_map);
        model_shader->setTexture("material.diffuse_map", 0);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, default_map);
        model_shader->setTexture("material.specular_map", 1);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, default_map);
        model_shader->setTexture("material.normal_map", 2);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
        model_shader->setTexture("shadowMap", 7);
        glActiveTexture(GL_TEXTURE0);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, ground.get_model_matrix());
            renderer.draw(*depth_shader, ground.get_mesh().get_VAO(), DrawMode::Indices, ground.get_mesh().get_indices_count());
        }else
            renderer.draw(*model_shader, ground.get_mesh().get_VAO(), DrawMode::Indices, ground.get_mesh().get_indices_count());
    }

    // render cube
    {
        model_shader->start_using();
        model_shader->setMatrix("model", 1, cube.get_model_matrix());
        model_shader->setFloat3("color", glm::vec3(1.0f));
        model_shader->setFloat("magnitude", magnitude);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.material().diffuse_map);
        model_shader->setTexture("material.diffuse_map", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.material().specular_map);
        model_shader->setTexture("material.specular_map", 1);
        glActiveTexture(GL_TEXTURE0);
        model_shader->setFloat("material.shininess", cube.get_material().shininess);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, cube.get_model_matrix());
            renderer.draw(*depth_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());
        }else
            renderer.draw(*model_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());


        // render border
        //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        //border_shader->start_using();
        //auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
        //border_shader->setMatrix("model", 1, cube.get_model_matrix() * scale * glm::mat4(1.0f));
        //border_shader->setMatrix("view", 1, camera->get_view());
        //border_shader->setMatrix("projection", 1, camera->get_projection());
        //renderer.draw(*border_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());
    }

    // render model
    {
        model_shader->start_using();
        auto nanosuit_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));
        auto nanosuit_translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        model_shader->setMatrix("model", 1, nanosuit_translate * nanosuit_scale);
        model_shader->setFloat3("color", glm::vec3(1.0f));
        model_shader->setFloat("magnitude", magnitude);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
        model_shader->setTexture("shadowMap", 7);
        glActiveTexture(GL_TEXTURE0);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, nanosuit_translate * nanosuit_scale);
            nanosuit.draw(*depth_shader, renderer);
        }else
            nanosuit.draw(*model_shader, renderer);
    }

    {
        model_shader->start_using();
        auto yoko_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
        auto yoko_translate = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
        model_shader->setMatrix("model", 1, yoko_translate * yoko_scale);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, yoko_translate * yoko_scale);
            yoko.draw(*depth_shader, renderer);
        }else
            yoko.draw(*model_shader, renderer);
    }
}

void render_imgui() {
    auto camera = scene.camera();
    auto& light = *scene.object(0);
    auto& cube = *scene.object(1);
    auto& ground = *scene.object(2);

    ImGui::Begin("Controller");

    if (ImGui::Checkbox("pixel style", &pixel_style));

    ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("cube shininess", &cube.material().shininess, 0, 1.5f);

    ImGui::SliderFloat("ground diffuse strength", &ground.material().diffuse_strength, 0, 1);
    ImGui::SliderFloat("ground specular strength", &ground.material().specular_strength, 0, 1);
    ImGui::SliderFloat("ground shininess", &ground.material().shininess, 0, 256);

    ImGui::SliderFloat("magnitude", &magnitude, 0.0f, 10.0f);

    ImGui::PushItemWidth(85.0f);
    static glm::vec3 cube_offset = { 0.0f, 1.0f, 0.0f };
    ImGui::SliderFloat("##cube x", &cube_offset.x, -10.0f, 10.0f);
    ImGui::SameLine();
    ImGui::SliderFloat("##cube y", &cube_offset.y, -10.0f, 10.0f);
    ImGui::SameLine();
    ImGui::SliderFloat("cube xyz", &cube_offset.z, -10.0f, 10.0f);
    auto translate = glm::translate(glm::mat4(1.0f), cube_offset);
    cube.set_model_matrix(translate);
    ImGui::PopItemWidth();
    //ImGui::ColorEdit3("light color", (float*)&light_color);
    //ImGui::ColorEdit3("cube color", (float*)&cube_color);
    //ImGui::ColorEdit3("ground color", (float*)&ground_color);

    if (ImGui::Checkbox("stop rotate", &stop_rotate));

    // log
    {
        ImGui::NewLine();
        ImGui::Text("light matrix:");
        std::string test_light = matrix_log(light.get_model_matrix());
        ImGui::Text(test_light.c_str());

        ImGui::NewLine();
        ImGui::Text("cube matrix:");
        std::string test_cube = matrix_log(cube.get_model_matrix());
        ImGui::Text(test_cube.c_str());

        //ImGui::NewLine();
        //ImGui::Text("ground matrix:");
        //std::string test_ground = matrix_log(ground.get_model_matrix());
        //ImGui::Text(test_ground.c_str());

        ImGui::NewLine();
        ImGui::Text("view matrix:");
        std::string test_view = matrix_log(camera->get_view());
        ImGui::Text(test_view.c_str());

        ImGui::NewLine();
        ImGui::Text("inverse view matrix:");
        std::string inverse_view = matrix_log(glm::inverse(camera->get_view()));
        ImGui::Text(inverse_view.c_str());

        ImGui::NewLine();
        ImGui::Text("camera position:");
        std::string test_camera_pos = vec3_log(camera->get_position());
        ImGui::Text(test_camera_pos.c_str());

        ImGui::NewLine();
        ImGui::Text("camera direction:");
        std::string test_camera_dir = vec3_log(camera->get_direction().dir);
        ImGui::Text(test_camera_dir.c_str());

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }

    ImGui::End();
}

// TODO:
// done: nanosuit 放在cube前加载就有问题: 答: body是nanosuit的最后一个材质，可能被cube覆盖了？已经无法复现。
// done: yoko 和 nanosuit 同时加载就有问题: 答: yoko模型没有镜面贴图，使用了nanosuit的镜面贴图导致的，加上default_map解决了
// done: main.cpp全局变量优化 答: 已将Object渲染对象和Camera交由Scene管理
// done: loop中的逻辑分离 答: 分离了imgui的渲染
// done: 材质应该包含shader
// 1. 键盘和鼠标callback事件用imgui处理? 交由View对象管理?
// 6. model.gs explode效果跟着fov缩放变
// 7. 着色器的坐标空间理解和统一
// 8. Model类Mesh和Material对应关系处理
// 9. 相机旋转和平移和缩放（鼠标左键右键滚轮）多次操作会出现问题，缩放不可逆。相机的拖动导致距离渲染对象变近
// 10. 几何着色器法向量显示
// 11. 光源物理模型
// 12. 阴影贴图、帧缓冲、法线贴图、tbn矩阵、天空盒、反射等知识学习
// 13. 帧缓冲的附件?
// 14. 看一下模型加载那篇文章，贴图文件是相对路径保存的和绝对路径保存的，Assimp的简单原理。

unsigned int creat_quad() {
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    return quadVAO;
}

void start_render_loop(GLFWwindow* window) {
    Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    // debug
    Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
    Renderer renderer;

    unsigned int quad_VAO = creat_quad();

    // 创建帧缓冲
    unsigned int frame_buffer;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    unsigned int tex_color_buffer;
    glGenTextures(1, &tex_color_buffer);
    glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0);
    // 为什么要绑上深度缓冲才能work? tex_buffer不是已经被深度测试过的一张纹理吗
    // 为什么只绑定了stencil，阴影就会有问题？
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    //glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    ////创建深度缓冲（阴影）
    unsigned int depth_frame_buffer;
    glGenFramebuffers(1, &depth_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
    glGenTextures(1, &tex_depth_buffer);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth_buffer, 0);
    //不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);// 为了渲染border
    glStencilMask(0xFF);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数

        // 1. 生成深度缓冲
        float depth_buffer_width = WINDOW_WIDTH;
        float depth_buffer_height = WINDOW_HEIGHT;
        set_view_port(depth_buffer_width, depth_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render_scene(depth_shader);

            // debug depth
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            //glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
            //renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);
        
        // 2. 生成颜色缓冲
        float color_buffer_width = WINDOW_WIDTH;
        float color_buffer_height = WINDOW_HEIGHT;
        if (pixel_style) {
            color_buffer_width /= 8.0f;
            color_buffer_height /= 8.0f;
        }
        glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_buffer_width, color_buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        set_view_port(color_buffer_width, color_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render_scene(nullptr);

        // 颜色缓冲和深度缓冲使用同一个framebuffer，第3步渲染时阴影没了，为什么？
        // 3. 默认缓冲
        set_view_port(WINDOW_WIDTH, WINDOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
        renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);

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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    init_scene();
    start_render_loop(window);

    // shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}