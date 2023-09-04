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
    //glfwInit��������ʼ��GLFW��glfwWindowHint����������GLFW
    glfwInit();
    //3.3�汾�Դ�����ϸ��ˣ����磺�����Լ�ָ�����Ƹ�glBindBuffer(),����Ҫʹ��glGenBuffers�����Ƿ��ص����ơ�
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����GLFW����Ҫʹ�õ�OpenGL�汾��3.3,����GLFW���ڴ���OpenGL������ʱ�����ʵ��ĵ���
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//��ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//���Ҳ������û��������ڵĴ�С

    //����һ�����ڶ���
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "RenderEngine", nullptr, nullptr);
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
// done: nanosuit ����cubeǰ���ؾ�������: ��: body��nanosuit�����һ�����ʣ����ܱ�cube�����ˣ��Ѿ��޷����֡�
// done: yoko �� nanosuit ͬʱ���ؾ�������: ��: yokoģ��û�о�����ͼ��ʹ����nanosuit�ľ�����ͼ���µģ�����default_map�����
// done: main.cppȫ�ֱ����Ż� ��: �ѽ�����Object��Ⱦ������Scene����
// done: loop �е��߼����� ��: ������imgui����Ⱦ
// done: ����Ӧ�ð���shader
// ��ɫ�������Ȼ���ʹ��ͬһ��framebuffer����3����Ⱦʱ��Ӱû�ˣ�Ϊʲô�� 
// ע�⿪�����ػ����ܿ���λ�ò��Ե���Ӱ
// done: model.gs �� explode Ч������fov���ű� ��ԭ��learnOpengl��gs�㷨�ڲü��ռ���㷨��������ͶӰ�任�Ƿ������任��
// done: ��ɫ��������ռ�����ͳһ
// done: ���̺����callback�¼���imgui����
// done: ������ɫ����������ʾ
// done: �����¼�������ƶ�����Ӧ�������������ϵ����
// done����ǰ����϶��ӽ�ΪFPS style,�ᵼ�������ת��ƽ�ƶ�β�����������⣬������϶����¾�����Ⱦ����������Ҫʵ���϶��ӽ��������һ��Բ�������
// done: �ذ�����
// done: ����View��Scene����ְ�� �𣺲ο�filament
// done: Model��Mesh��Material��Ӧ��ϵ����
// done: picking
// done: ʵ��ECS�ܹ���Component��������������main.cpp����ȫ�ֱ�����װ�������߼�����
// 10. �������˶���������(direction = (0,-1,0)ʱ)
// 21. depth_shader�����Ƴ���ֱ����ͬһ��shader����ͬview���󼴿�
// 11. ��Դ����ģ��
// 12. ��Ӱ��ͼ(done)��֡����(done)��������ͼ��tbn������պ�(done)������(done)��֪ʶѧϰ
// 13. ֡����ĸ���?
// 14. ��һ��ģ�ͼ�����ƪ���£���ͼ�ļ������·������Ļ����·������ģ�Assimp�ļ�ԭ��
// 16. ����ϵͳ
// 18. low-poly
// 19. ��պеĲ�������ɫ����������ϵ���
// 19. ��պе�ģ�;����С������ʲô
// 19. ��ǰ�����㷨�����㣺���벻��
// 20. ��̬������ͼ������ʱ
// 22. ���ڴ�С������
// 23. ����Ⱦ��
// 24. ѧ��renderdocʹ��
// 25. �Ż�cpu��gpu��io����
// 26. Editor �༭���ʺ󣬱����ļ�

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
    // TODO ��취�Ž�view����shadow_map���߼�
    Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    unsigned int quad_VAO = creat_quad();
    Shader* frame_shader = new Shader("resource/shader/frame.vs", "resource/shader/frame.fs");
    Renderer renderer;

    // ����֡����
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
    // ΪʲôҪ������Ȼ������work? tex_buffer�����Ѿ�����Ȳ��Թ���һ��������
    // Ϊʲôֻ����stencil����Ӱ�ͻ������⣿
    //unsigned int rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    ////glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    ////glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //������Ȼ��壨��Ӱ��
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
    //��������ɫ�����֡��������ǲ������ģ�����������Ҫ��ʽ����OpenGL���ǲ������κ���ɫ���ݽ�����Ⱦ��
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ����picking֡����
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
    glEnable(GL_STENCIL_TEST);// Ϊ����Ⱦborder
    glStencilMask(0xFF);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();//��鴥���¼�������������롢����ƶ��ȣ���Ȼ����ö�Ӧ�Ļص�����
        view.mouse_and_key_callback();

        // 1. ������Ȼ���
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
            
        // 2. ������ɫ����
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

        // 3. Ĭ�ϻ���
        //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //glBindTexture(GL_TEXTURE_2D, picking_texture);
        //renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);

        editor.render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }
}

int main()
{
    GLFWwindow* window = create_window((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // FPS ģʽ

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