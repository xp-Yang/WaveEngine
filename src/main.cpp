#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Object.hpp"
#include "Skybox.hpp"
#include "MyCube.hpp"
#include "MySphere.hpp"
#include "MyLight.hpp"
#include "MyGround.hpp"
#include "MyModel.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "stb_image.h"
#include <stdio.h>
#include <map>

Scene scene;
View view;

std::string matrix_log(const glm::mat4 mat)
{
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].x, mat[1].x, mat[2].x, mat[3].x);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].y, mat[1].y, mat[2].y, mat[3].y);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].z, mat[1].z, mat[2].z, mat[3].z);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].w, mat[1].w, mat[2].w, mat[3].w);
    result += buf;
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

std::string vec3_log(const glm::vec3 vec) {
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f \n", vec.x, vec.y, vec.z);
    result += buf;
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

void imgui_callback() 
{
    ImGuiIO& io = ImGui::GetIO();
    auto& camera = view.get_camera();

    double xpos, ypos;
    xpos = io.MousePos.x;
    ypos = io.MousePos.y;
    if (io.WantCaptureMouse) {
        return;
    }
    static bool last_left_mouse_status = io.MouseDown[0];
    static bool last_right_mouse_status = io.MouseDown[1];

    static float last_pos_x = WINDOW_WIDTH / 2;
    static float last_pos_y = WINDOW_HEIGHT / 2;
    if (last_left_mouse_status != io.MouseDown[0]) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_left_mouse_status = io.MouseDown[0];
    }
    if (io.MouseDown[0]) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y);
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 0);
    }

    if (last_right_mouse_status != io.MouseDown[1]) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_right_mouse_status = io.MouseDown[1];
    }
    if (io.MouseDown[1]) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y);
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 1);
    }

    camera.mouse_scroll_process(io.MouseWheel);

    float delta_time = 1.0f / io.Framerate;
    if (io.KeyShift)
        delta_time /= 10.0f;
    if (io.KeysDown['W']) {
        camera.key_process('W', delta_time);
    }
    if (io.KeysDown['A']) {
        camera.key_process('A', delta_time);
    }
    if (io.KeysDown['S']) {
        camera.key_process('S', delta_time);
    }
    if(io.KeysDown['D']) {
        camera.key_process('D', delta_time);
    }
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

void init_scene() {
    Skybox* skybox = new Skybox();
    Material skybox_material;
    skybox_material.shader = new Shader("resource/shader/skybox.vs", "resource/shader/skybox.fs");
    skybox_material.diffuse_map = skybox->get_texture_id();
    skybox_material.diffuse_map_path = "resource/images/skybox/test.jpg";
    skybox->set_material(skybox_material);

    MyLight* light = new MyLight();
    Material light_material;
    light_material.shader = new Shader("resource/shader/light.vs", "resource/shader/light.fs");
    light->set_material(light_material);

    MyCube* cube = new MyCube();
    Material cube_material;
    cube_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    cube_material.set_diffuse_map("resource/images/default_white_map.png");
    cube_material.set_specular_map("resource/images/cube_specular.png");
    cube->set_material(cube_material);

    MySphere* sphere = new MySphere();
    Material sphere_material;
    sphere_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    sphere_material.set_diffuse_map("resource/images/default_white_map.png");
    sphere_material.set_specular_map("resource/images/cube_specular.png");
    sphere->set_material(sphere_material);

    MyGround* ground = new MyGround();
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.set_diffuse_map("resource/images/desert.jpg");
    ground_material.set_specular_map("resource/images/desert.jpg");
    ground->set_material(ground_material);

    Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
    Shader* model_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    for (auto& material : nanosuit->m_materials) {
        material.shader = model_shader;
    }
    Material nanosuit_material;
    nanosuit_material.shader = model_shader;
    nanosuit->set_material(nanosuit_material);

    Model* yoko = new Model("resource/model/yoko/008.obj");
    for (auto& material : yoko->m_materials) {
        material.shader = model_shader;
    }
    Material yoko_material;
    yoko_material.shader = model_shader;
    yoko->set_material(yoko_material);

    scene.insert_object("cube", cube);
    scene.insert_object("sphere", sphere);
    scene.insert_object("ground", ground);
    scene.insert_object("nanosuit", nanosuit);
    scene.insert_object("yoko", yoko);
    scene.set_skybox(skybox);
    scene.set_light(light);
}

void init_view() {
    glm::vec3 camera_pos(0.0f, 15.0f, 15.0f);
    Camera* camera = new Camera(camera_pos, glm::vec3(0.0f) - camera_pos);
    view.set_camera(camera);
    view.set_scene(&scene);
}

// TODO remove
static bool pixel_style = false;
static bool stop_rotate = false;
static bool normal_debug = false;
static float ambient_strength = 0.1f;
static int icosphere_accuracy = 8;
static float magnitude = 0.0f;
unsigned int tex_depth_buffer;

void render_scene(Shader* depth_shader, Shader* normal_shader) {
    Camera& camera = view.get_camera();
    Skybox& skybox = *const_cast<Skybox*>(&scene.get_skybox());
    auto& light = *const_cast<MyLight*>(&scene.get_light());
    auto& cube = *scene.object("cube");
    MySphere& sphere = *static_cast<MySphere*>(scene.object("sphere"));
    auto& ground = *scene.object("ground");
    Model& nanosuit = *static_cast<Model*>(scene.object("nanosuit"));
    Model& yoko = *static_cast<Model*>(scene.object("yoko"));

    Shader* skybox_shader = skybox.material().shader;
    Shader* light_shader = light.material().shader;
    Shader* cube_shader = cube.material().shader;
    Shader* sphere_shader = sphere.material().shader;
    Shader* ground_shader = ground.material().shader;
    Shader* model_shader = nanosuit.m_materials[0].shader;

    Renderer renderer;

    glm::mat4 lightProjection = glm::ortho(-10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -5.0f, 15.0f, 0.1f, 100.0f);
    //lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 light_pos = light.get_model_matrix()[3];
    glm::mat4 lightView = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    if (depth_shader) {
        depth_shader->start_using();
        depth_shader->setMatrix("view", 1, lightSpaceMatrix);
    }

    light_shader->start_using();
    light_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
    light_shader->setMatrix("view", 1, camera.get_view());
    light_shader->setMatrix("projection", 1, camera.get_projection());
    light_shader->setFloat3("color", light.get_material().color);

    normal_shader->start_using();
    normal_shader->setMatrix("view", 1, camera.get_view());
    normal_shader->setMatrix("projection", 1, camera.get_projection());

    skybox_shader->start_using();
    auto skybox_view = camera.get_view();
    skybox_view = glm::mat4(glm::mat3(skybox_view));
    skybox_shader->setMatrix("view", 1, skybox_view);
    skybox_shader->setMatrix("projection", 1, camera.get_projection());

    // render skybox
    {
        glDepthMask(GL_FALSE);
        skybox_shader->start_using();
        skybox_shader->setMatrix("model", 1, skybox.get_model_matrix());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_texture_id());
        skybox_shader->setTexture("skybox", 6);
        renderer.draw(*skybox_shader, skybox.get_mesh().get_VAO(), DrawMode::Indices, skybox.get_mesh().get_indices_count());
        glDepthMask(GL_TRUE);
    }

    // render light
    {
        light_shader->start_using();
        static float time_value = 0.0f;
        if (stop_rotate) {
            ;
        }
        if (!stop_rotate) {
            time_value = glfwGetTime();
        }
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 displacement(1.0f);
        displacement[3].x = 7.0f;
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value * 3, glm::vec3(0.0f, 1.0f, 0.0f));
        auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 5.0f, 0.0f });
        light.set_model_matrix(translate * rotate * displacement * scale);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, light.get_model_matrix());
            renderer.draw(*depth_shader, light.get_mesh().get_VAO(), DrawMode::Indices, light.get_mesh().get_indices_count());
        }else
            renderer.draw(*light_shader, light.get_mesh().get_VAO(), DrawMode::Indices, light.get_mesh().get_indices_count());
    }

    // render sphere
    {
        if (icosphere_accuracy != sphere.get_recursive_depth()) {
            sphere.create_icosphere(icosphere_accuracy);
            icosphere_accuracy = sphere.get_recursive_depth();
        }
        sphere_shader->start_using();
        sphere_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
        sphere_shader->setFloat("material.ambient", ambient_strength);
        auto sphere_translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 2.0f));
        sphere.set_model_matrix(sphere_translate * glm::mat4(1.0f));
        sphere.material().update_shader_binding();

        // debug
        //glBindVertexArray(sphere.get_mesh().get_VAO());
        //glDrawArrays(GL_LINE_LOOP, 0, sphere.get_vertices_count());
        //glBindVertexArray(0);

        //renderer.draw(*sphere_shader, sphere.get_mesh().get_VAO(), DrawMode::Arrays, 0, sphere.get_vertices_count());

        if (normal_debug) {
            normal_shader->start_using();
            normal_shader->setMatrix("model", 1, sphere.get_model_matrix());
            normal_shader->setFloat("magnitude", magnitude);
            renderer.draw(*normal_shader, sphere.get_mesh().get_VAO(), DrawMode::Arrays, 0, sphere.get_vertices_count());
        }
    }

    // render ground
    {
        ground_shader->start_using();
        ground_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
        ground_shader->setFloat("material.ambient", ambient_strength);
        ground_shader->setFloat("magnitude", 0);
        ground.material().update_shader_binding();

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, ground.get_model_matrix());
            renderer.draw(*depth_shader, ground.get_mesh().get_VAO(), DrawMode::Indices, ground.get_mesh().get_indices_count());
        }
        //else
        //    renderer.draw(*ground_shader, ground.get_mesh().get_VAO(), DrawMode::Indices, ground.get_mesh().get_indices_count());
    }

    // render cube
    {
        cube_shader->start_using();
        cube_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
        cube_shader->setFloat("material.ambient", ambient_strength);
        cube_shader->setFloat("magnitude", magnitude);
        cube.material().update_shader_binding();

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, cube.get_model_matrix());
            renderer.draw(*depth_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());
        }
        else {
            //renderer.draw(*cube_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());

            if (normal_debug) {
                normal_shader->start_using();
                normal_shader->setMatrix("model", 1, cube.get_model_matrix());
                normal_shader->setFloat("magnitude", magnitude);
                renderer.draw(*normal_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());
            }
        }

        // render border
        //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        //border_shader->start_using();
        //auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
        //border_shader->setMatrix("model", 1, cube.get_model_matrix() * scale * glm::mat4(1.0f));
        //border_shader->setMatrix("view", 1, camera.get_view());
        //border_shader->setMatrix("projection", 1, camera.get_projection());
        //renderer.draw(*border_shader, cube.get_mesh().get_VAO(), DrawMode::Indices, cube.get_mesh().get_indices_count());
    }

    renderer.render(view);

    // render model
    model_shader->start_using();
    model_shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
    model_shader->setFloat("material.ambient", ambient_strength);
    {
        model_shader->start_using();
        auto nanosuit_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));
        auto nanosuit_translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        model_shader->setMatrix("model", 1, nanosuit_translate * nanosuit_scale);
        model_shader->setFloat("magnitude", magnitude);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
        model_shader->setTexture("shadow_map", 7);
        glActiveTexture(GL_TEXTURE0);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, nanosuit_translate * nanosuit_scale);
            nanosuit.draw(*depth_shader, renderer);
        }
        else {
            nanosuit.draw(*model_shader, renderer);

            if (normal_debug) {
                normal_shader->start_using();
                normal_shader->setMatrix("model", 1, nanosuit_translate * nanosuit_scale);
                normal_shader->setFloat("magnitude", magnitude);
                nanosuit.draw(*normal_shader, renderer);
            }
        }
    }

    {
        model_shader->start_using();
        auto yoko_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
        auto yoko_translate = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
        model_shader->setMatrix("model", 1, yoko_translate * yoko_scale);
        model_shader->setFloat("magnitude", magnitude);

        if (depth_shader) {
            depth_shader->start_using();
            depth_shader->setMatrix("model", 1, yoko_translate * yoko_scale);
            yoko.draw(*depth_shader, renderer);
        }
        else {
            yoko.draw(*model_shader, renderer);

            if (normal_debug) {
                normal_shader->start_using();
                normal_shader->setMatrix("model", 1, yoko_translate * yoko_scale);
                normal_shader->setFloat("magnitude", magnitude);
                yoko.draw(*normal_shader, renderer);
            }
        }
    }
}

void render_imgui() {
    auto& camera = view.get_camera();
    auto& light = *const_cast<MyLight*>(&scene.get_light());
    auto& cube = *scene.object("cube");
    auto& ground = *scene.object("ground");

    ImGui::Begin("Controller");

    if (ImGui::Checkbox("pixel style", &pixel_style));

    ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("model(except ground) shininess", &cube.material().shininess, 0, 512.f);

    //ImGui::SliderFloat("ground diffuse strength", &ground.material().diffuse_strength, 0, 1);
    //ImGui::SliderFloat("ground specular strength", &ground.material().specular_strength, 0, 1);
    ImGui::SliderFloat("ground shininess", &ground.material().shininess, 0, 512.f);

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
    ImGui::SliderInt("icosphere accuracy", &icosphere_accuracy, 0, 10);
    //ImGui::ColorEdit3("light color", (float*)&light_color);
    //ImGui::ColorEdit3("cube color", (float*)&cube_color);
    //ImGui::ColorEdit3("ground color", (float*)&ground_color);

    if (ImGui::Checkbox("stop rotate", &stop_rotate));
    if (ImGui::Checkbox("open normal debug", &normal_debug));

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
        std::string test_view = matrix_log(camera.get_view());
        ImGui::Text(test_view.c_str());

        ImGui::NewLine();
        ImGui::Text("inverse view matrix:");
        std::string inverse_view = matrix_log(glm::inverse(camera.get_view()));
        ImGui::Text(inverse_view.c_str());

        ImGui::NewLine();
        ImGui::Text("camera position:");
        std::string test_camera_pos = vec3_log(camera.get_position());
        ImGui::Text(test_camera_pos.c_str());

        ImGui::NewLine();
        ImGui::Text("camera direction:");
        std::string test_camera_dir = vec3_log(camera.get_direction().dir);
        ImGui::Text(test_camera_dir.c_str());

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }

    ImGui::End();
}

// TODO:
// done: nanosuit 放在cube前加载就有问题: 答: body是nanosuit的最后一个材质，可能被cube覆盖了？已经无法复现。
// done: yoko 和 nanosuit 同时加载就有问题: 答: yoko模型没有镜面贴图，使用了nanosuit的镜面贴图导致的，加上default_map解决了
// done: main.cpp全局变量优化 答: 已将所有Object渲染对象和Camera交由Scene管理
// done: loop中的逻辑分离 答: 分离了imgui的渲染
// done: 材质应该包含shader
// 颜色缓冲和深度缓冲使用同一个framebuffer，第3步渲染时阴影没了，为什么？ 
// 注意开启像素化后能看见位置不对的阴影
// done: model.gs 的 explode 效果跟着fov缩放变 答：原因：learnOpengl的gs算法在裁剪空间计算法向量，而投影变换是非正交变换。
// done: 着色器的坐标空间理解和统一
// done: 键盘和鼠标callback事件用imgui处理
// done: 几何着色器法向量显示
// done: 键盘事件摄像机移动方向应该是摄像机坐标系方向
// done：当前相机拖动视角为FPS style,会导致相机旋转和平移多次操作会出现问题，相机的拖动导致距离渲染对象变近。需要实现拖动视角令相机在一个圆轨道运行
// done: 地板铺满
// 6. 解决相机运动死锁问题(direction = (0,-1,0)时)
// 7. 定义View、Scene对象职责
// 8. Model类Mesh和Material对应关系处理
// 11. 光源物理模型
// 12. 阴影贴图(done)、帧缓冲(done)、法线贴图、tbn矩阵、天空盒(done)、反射(done)等知识学习
// 13. 帧缓冲的附件?
// 14. 看一下模型加载那篇文章，贴图文件是相对路径保存的或绝对路径保存的，Assimp的简单原理。
// 15. picking
// 16. 粒子系统
// 18. low-poly
// 19. 天空盒的采样和着色器采样坐标系理解
// 19. 天空盒的模型矩阵大小代表着什么
// 19. 当前反射算法的弱点：距离不对
// 20. 动态环境贴图，球面时
// 21. depth_shader可以移出，直接用同一个shader传不同view矩阵即可
// 22. 窗口大小可缩放
// 23. 批渲染？

unsigned int creat_quad() {
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions          // uv
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
                              
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f
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
    Shader* normal_shader = new Shader("resource/shader/model.vs", "resource/shader/normal.fs", "resource/shader/normal.gs");

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
    //unsigned int rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    ////glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    ////glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);

    ////创建深度缓冲（阴影）
    //unsigned int depth_frame_buffer;
    //glGenFramebuffers(1, &depth_frame_buffer);
    //glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
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
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
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
        imgui_callback();

        // 1. 生成深度缓冲
        float depth_buffer_width = WINDOW_WIDTH;
        float depth_buffer_height = WINDOW_HEIGHT;
        glViewport(0, 0, depth_buffer_width, depth_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render_scene(depth_shader, normal_shader);

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
            color_buffer_width /= 6.0f;
            color_buffer_height /= 6.0f;
        }
        glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_buffer_width, color_buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glViewport(0, 0, color_buffer_width, color_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render_scene(nullptr, normal_shader);

        // 3. 默认缓冲
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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
    init_view();
    start_render_loop(window);

    // shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}