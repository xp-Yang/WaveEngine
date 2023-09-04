#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Object.hpp"
#include "Skybox.hpp"
#include "MyCube.hpp"
#include "MySphere.hpp"
#include "MyLight.hpp"
#include "MyGround.hpp"
#include "MyModel.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "View.hpp"
#include "Scene.hpp"
#include "stb_image.h"
#include "ECS/World.hpp"
#include "ECS/Components.hpp"
#include "Editor/ImGuiEditor.hpp"
#include "Logger.hpp"

Scene scene;
View view;
ImGuiEditor editor;

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

void render_shadow_map(Shader* depth_shader) {
    Skybox& skybox = *const_cast<Skybox*>(&scene.get_skybox());
    auto& light = *const_cast<MyLight*>(&scene.get_light());
    auto& cube = *scene.object("cube");
    MySphere& sphere = *static_cast<MySphere*>(scene.object("sphere"));
    auto& ground = *scene.object("ground");
    Model& nanosuit = *static_cast<Model*>(scene.object("nanosuit"));
    Model& yoko = *static_cast<Model*>(scene.object("yoko"));

    Renderer renderer;

    glm::mat4 lightSpaceMatrix = light.get_light_space_matrix();

    depth_shader->start_using();
    depth_shader->setMatrix("view", 1, lightSpaceMatrix);

    depth_shader->setMatrix("model", 1, light.get_model_matrix());
    renderer.drawIndex(*depth_shader, light.mesh().get_VAO(), light.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, sphere.get_model_matrix());
    renderer.drawTriangle(*depth_shader, sphere.mesh().get_VAO(), sphere.mesh().get_vertices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, cube.get_model_matrix());
    renderer.drawIndex(*depth_shader, cube.mesh().get_VAO(), cube.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, ground.get_model_matrix());
    renderer.drawIndex(*depth_shader, ground.mesh().get_VAO(), ground.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, nanosuit.get_model_matrix());
    nanosuit.draw(*depth_shader, renderer);

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, yoko.get_model_matrix());
    yoko.draw(*depth_shader, renderer);
}

// TODO:
// done: nanosuit 放在cube前加载就有问题: 答: body是nanosuit的最后一个材质，可能被cube覆盖了？已经无法复现。
// done: yoko 和 nanosuit 同时加载就有问题: 答: yoko模型没有镜面贴图，使用了nanosuit的镜面贴图导致的，加上default_map解决了
// done: main.cpp全局变量优化 答: 已将所有Object渲染对象交由Scene管理
// done: loop 中的逻辑分离 答: 分离了imgui的渲染
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
// done: 定义View、Scene对象职责 答：参考filament
// done: Model类Mesh和Material对应关系处理
// done: picking
// done: 实现ECS架构，Component数据驱动开发，main.cpp所有全局变量封装，所有逻辑分离
// 10. 解决相机运动死锁问题(direction = (0,-1,0)时)
// 21. depth_shader可以移出，直接用同一个shader传不同view矩阵即可
// 11. 光源物理模型
// 12. 阴影贴图(done)、帧缓冲(done)、法线贴图、tbn矩阵、天空盒(done)、反射(done)等知识学习
// 13. 帧缓冲的附件?
// 14. 看一下模型加载那篇文章，贴图文件是相对路径保存的或绝对路径保存的，Assimp的简单原理。
// 16. 粒子系统
// 18. low-poly
// 19. 天空盒的采样和着色器采样坐标系理解
// 19. 天空盒的模型矩阵大小代表着什么
// 19. 当前反射算法的弱点：距离不对
// 20. 动态环境贴图，球面时
// 22. 窗口大小可缩放
// 23. 批渲染？
// 24. 学会renderdoc使用
// 25. 优化cpu和gpu的io调用
// 26. Editor 编辑材质后，保存文件

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
    // TODO 想办法放进view开启shadow_map的逻辑
    Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    unsigned int quad_VAO = creat_quad();
    Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
    Renderer renderer;

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
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //创建深度缓冲（阴影）
    unsigned int depth_FBO;
    glGenFramebuffers(1, &depth_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
    unsigned int depth_texture;
    glGenTextures(1, &depth_texture);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    //不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 创建picking帧缓冲
    //unsigned int picking_FBO;
    //glGenFramebuffers(1, &picking_FBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, picking_FBO);
    //view.set_picking_FBO(picking_FBO);
    //unsigned int picking_texture;
    //glActiveTexture(GL_TEXTURE8);
    //glGenTextures(1, &picking_texture);
    //glBindTexture(GL_TEXTURE_2D, picking_texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);
    //unsigned int picking_depth_texture;
    //glGenTextures(1, &picking_depth_texture);
    //glActiveTexture(GL_TEXTURE7);
    //glBindTexture(GL_TEXTURE_2D, picking_depth_texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, picking_depth_texture, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);// 为了渲染border
    glStencilMask(0xFF);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数
        view.mouse_and_key_callback();

        // 1. 生成深度缓冲
        float depth_buffer_width = WINDOW_WIDTH;
        float depth_buffer_height = WINDOW_HEIGHT;
        glViewport(0, 0, depth_buffer_width, depth_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render_shadow_map(depth_shader);
        view.set_shadow_map_id(depth_texture);

            // debug depth
            //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            //glBindTexture(GL_TEXTURE_2D, view.get_shadow_map_id());
            //renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);
            
        // 2. 生成颜色缓冲
        float color_buffer_width = WINDOW_WIDTH;
        float color_buffer_height = WINDOW_HEIGHT;
        if (editor.pixel_style) {
            color_buffer_width /= 6.0f;
            color_buffer_height /= 6.0f;
        }
        glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_buffer_width, color_buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glViewport(0, 0, color_buffer_width, color_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(editor.ambient_strength * 0.5f, editor.ambient_strength * 0.5f, editor.ambient_strength * 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        view.enable_shadow_map(true);
        //renderer.render(view);
        renderer.render_ecs(view);

        if (editor.normal_debug)
            renderer.render_normal(view);

        // 3. 默认缓冲
        //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //glBindTexture(GL_TEXTURE_2D, picking_texture);
        //renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);

        editor.render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并输出显示在屏幕上。
    }
}

int main()
{
    GLFWwindow* window = create_window((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);
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

    scene.init();

    view.set_scene(&scene);

    start_render_loop(window);

    // shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}