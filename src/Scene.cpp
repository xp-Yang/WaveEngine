#include "Scene.hpp"
#include "MyCube.hpp"
#include "MySphere.hpp"
#include "MyGround.hpp"
#include "MyModel.hpp"
//void screen_shot() {
//    const int channels_num = 3;
//    const int w = (int)WINDOW_WIDTH;
//    const int h = (int)WINDOW_HEIGHT;
//
//    unsigned char data[w * h * channels_num];
//
//    glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, data);
//
//    int index = 0;
//    for (int j = h - 1; j >= 0; --j)
//    {
//        for (int i = 0; i < w; ++i)
//        {
//            data[index++] = (unsigned char)(255.0 * i / w);
//            data[index++] = (unsigned char)(255.0 * j / h);
//            data[index++] = (unsigned char)(255.0 * 0.2);
//        }
//    }
//
//    //stbi_write_jpg("jpg_test_.jpg", w, h, channels_num, data, w * channels_num);
//}

void Scene::init_scene() {
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
    sphere_material.set_specular_map("resource/images/default_white_map.png");
    sphere->set_material(sphere_material);

    MyGround* ground = new MyGround();
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.set_diffuse_map("resource/images/desert.jpg");
    ground_material.set_specular_map("resource/images/desert.jpg");
    ground->set_material(ground_material);

    Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
    Shader* nanosuit_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    for (auto& material : nanosuit->m_materials) {
        material.shader = nanosuit_shader;
    }
    Material nanosuit_material;
    nanosuit_material.shader = nanosuit_shader;
    nanosuit->set_material(nanosuit_material);

    Model* yoko = new Model("resource/model/yoko/008.obj");
    Shader* yoko_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    for (auto& material : yoko->m_materials) {
        material.shader = yoko_shader;
    }
    Material yoko_material;
    yoko_material.shader = yoko_shader;
    yoko->set_material(yoko_material);

    insert_object("cube", cube);
    insert_object("sphere", sphere);
    insert_object("ground", ground);
    insert_object("nanosuit", nanosuit);
    insert_object("yoko", yoko);
    set_skybox(skybox);
    set_light(light);
}