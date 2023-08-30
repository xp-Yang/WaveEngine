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

// TODO remove
static bool pixel_style = false;
static bool stop_rotate = false;
static bool normal_debug = false;
static float ambient_strength = 0.1f;
static int icosphere_accuracy = 8;

// TODO �Ժ���UI��ֱ�ӱ༭��Щ����
void configure_scene() {
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
    Shader* model_shader = nanosuit.material().shader;
    Shader* yoko_shader = yoko.material().shader;


    // skybox
    {
    }

    // light
    {
        static float time_value = 0.0f;
        if (stop_rotate) {
            ;
        }
        if (!stop_rotate) {
            time_value = glfwGetTime();
        }
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 displacement(1.0f);
        displacement[3].x = 12.0f;
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value * 3, glm::vec3(0.0f, 1.0f, 0.0f));
        auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 12.0f, 0.0f });
        light.set_model_matrix(translate * rotate * displacement * scale);
    }

    // sphere
    {
        if (icosphere_accuracy != sphere.get_recursive_depth()) {
            sphere.create_icosphere(icosphere_accuracy);
            icosphere_accuracy = sphere.get_recursive_depth();
        }
        auto sphere_translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 2.0f));
        sphere.set_model_matrix(sphere_translate * glm::mat4(1.0f));
    }

    // cube
    {
        glm::vec3 cube_offset = { 0.0f, 1.0f, 0.0f };
        auto translate = glm::translate(glm::mat4(1.0f), cube_offset);
        cube.set_model_matrix(translate); // TODO ��֪��Ϊʲô����Ҫ����һ��model_matrix���ܱ�ѡ��
    }

    // ground
    {
        ground.set_renderable(false);
    }

    // nanosuit
    {
        auto nanosuit_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));
        auto nanosuit_translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
        nanosuit.set_model_matrix(nanosuit_translate * nanosuit_scale);
    }

    // yoko
    {
        auto yoko_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
        auto yoko_translate = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
        yoko.set_model_matrix(yoko_translate * yoko_scale);
    }
}

void configure_object()
{
    static Object* last_obj = nullptr;
    Object* curr_obj = nullptr;
    std::string obj_name = "";
    for (auto& item : view.get_scene().get_objects()) {
        if (item.second) {
            if (item.second->is_picked()) {
                curr_obj = item.second;
                obj_name = item.first;
            }
        }
    }
    if (!curr_obj)
        return;

    bool visible = curr_obj->renderable();
    bool reflection = curr_obj->is_enable_reflection();
    glm::vec3 color = curr_obj->material().color;
    float shininess = curr_obj->material().shininess;
    glm::vec3 obj_offset = curr_obj->get_model_matrix()[3];
    float explosion_ratio = curr_obj->get_explostion_ratio();

    auto reset_param = [](Object* curr_obj) {
    };

    if (curr_obj != last_obj) {
        last_obj = curr_obj;
        reset_param(curr_obj);
    }

    ImGui::Begin((obj_name + " controller").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Checkbox((std::string("visible") + "##" + obj_name).c_str(), &visible);
    ImGui::Checkbox((std::string("enable reflection") + "##" + obj_name).c_str(), &reflection);
    ImGui::ColorEdit3((std::string("color") + "##" + obj_name).c_str(), (float*)&color);
    ImGui::SliderFloat((std::string("shininess") + "##" + obj_name).c_str(), &shininess, 0.1f, 512.0f);
    ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion_ratio, 0.0f, 10.0f);
    ImGui::PushItemWidth(85.0f);
    ImGui::SliderFloat((std::string("##x") + "##" + obj_name).c_str(), &obj_offset.x, -10.0f, 10.0f);
    ImGui::SameLine();
    ImGui::SliderFloat((std::string("##y") + "##" + obj_name).c_str(), &obj_offset.y, -10.0f, 10.0f);
    ImGui::SameLine();
    ImGui::SliderFloat((std::string("xyz") + "##" + obj_name).c_str(), &obj_offset.z, -10.0f, 10.0f);
    ImGui::PopItemWidth();
    // �༭�����������
    for (int i = 0; i < curr_obj->get_materials().size(); i++) {
        curr_obj->material(i).ambient_strength;
        curr_obj->material(i).color = color;
        curr_obj->material(i).shininess = shininess;
        //curr_obj->material(i).set_diffuse_map();
        //curr_obj->material(i).set_specular_map();
    }
    // �༭��������
    auto new_matrix = curr_obj->get_model_matrix();
    new_matrix[3] = glm::vec4(obj_offset, 1.0f);
    curr_obj->set_model_matrix(new_matrix);
    // �༭�ɼ���
    curr_obj->set_renderable(visible);
    // �Ƿ������������
    // �༭�����explosion ratio
    curr_obj->set_explostion_ratio(explosion_ratio);
    // �༭�����Ƿ�������
    curr_obj->enable_reflection(reflection);
    ImGui::End();
}

void render_imgui() {
    auto& camera = view.get_camera();
    auto& light = *const_cast<MyLight*>(&scene.get_light());
    auto& cube = *scene.object("cube");
    auto& ground = *scene.object("ground");

    ImGui::Begin("Global Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Checkbox("pixel style", &pixel_style));

    ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);

    //ImGui::SliderFloat("ground diffuse strength", &ground.material().diffuse_strength, 0, 1);
    //ImGui::SliderFloat("ground specular strength", &ground.material().specular_strength, 0, 1);
    ImGui::SliderFloat("ground shininess", &ground.material().shininess, 0, 512.f);

    ImGui::SliderInt("icosphere accuracy", &icosphere_accuracy, 0, 10);

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

    if (depth_shader) {
        depth_shader->start_using();
        depth_shader->setMatrix("view", 1, lightSpaceMatrix);
    }

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, light.get_model_matrix());
    renderer.draw(*depth_shader, light.mesh().get_VAO(), DrawMode::Indices, light.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, sphere.get_model_matrix());
    renderer.draw(*depth_shader, sphere.mesh().get_VAO(), DrawMode::Arrays, 0, sphere.mesh().get_vertices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, cube.get_model_matrix());
    renderer.draw(*depth_shader, cube.mesh().get_VAO(), DrawMode::Indices, cube.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, ground.get_model_matrix());
    renderer.draw(*depth_shader, ground.mesh().get_VAO(), DrawMode::Indices, ground.mesh().get_indices_count());

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, nanosuit.get_model_matrix());
    nanosuit.draw(*depth_shader, renderer);

    depth_shader->start_using();
    depth_shader->setMatrix("model", 1, yoko.get_model_matrix());
    yoko.draw(*depth_shader, renderer);
}

void render_normal() {
    static Shader* normal_shader = new Shader("resource/shader/model.vs", "resource/shader/normal.fs", "resource/shader/normal.gs");
    if (!normal_debug)
        return;
    auto& camera = view.get_camera();

    normal_shader->start_using();
    normal_shader->setMatrix("view", 1, camera.get_view());
    normal_shader->setMatrix("projection", 1, camera.get_projection());
    Renderer renderer;
    for (auto& item : view.get_scene().get_objects()) {
        if (item.second && item.second->renderable()) {
            Object& obj = *item.second;
            normal_shader->start_using();
            normal_shader->setMatrix("model", 1, obj.get_model_matrix());
            for (int i = 0; i < obj.get_meshes().size(); i++) {
                if (!obj.render_as_indices()) {
                    renderer.draw(*normal_shader, obj.mesh(i).get_VAO(), DrawMode::Arrays, 0, obj.mesh(i).get_vertices_count());
                }
                else {
                    renderer.draw(*normal_shader, obj.mesh(i).get_VAO(), DrawMode::Indices, obj.mesh(i).get_indices_count());
                }
            }
        }
    }
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
// 10. �������˶���������(direction = (0,-1,0)ʱ)
// 15. picking
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
// 21. depth_shader�����Ƴ���ֱ����ͬһ��shader����ͬview���󼴿�
// 22. ���ڴ�С������
// 23. ����Ⱦ��
// 24. ѧ��renderdocʹ��
// 25. �Ż�cpu��gpu��io����

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

        configure_object();

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
        if (pixel_style) {
            color_buffer_width /= 6.0f;
            color_buffer_height /= 6.0f;
        }
        glBindTexture(GL_TEXTURE_2D, tex_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_buffer_width, color_buffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glViewport(0, 0, color_buffer_width, color_buffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        view.enable_shadow_map(true);
        renderer.render(view);

        render_normal();

        // 3. Ĭ�ϻ���
        //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(ambient_strength * 0.5f, ambient_strength * 0.5f, ambient_strength * 0.5f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //glBindTexture(GL_TEXTURE_2D, picking_texture);
        //renderer.draw(*frame_shader, quad_VAO, DrawMode::Arrays, 0, 6);

        render_imgui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);//�����ύ����ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ��������ʾ����Ļ�ϡ�
    }
}

int main()
{
    GLFWwindow* window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    configure_scene();

    glm::vec3 camera_pos(0.0f, 15.0f, 15.0f);
    Camera* camera = new Camera(camera_pos, glm::vec3(0.0f) - camera_pos);
    view.set_camera(camera);
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