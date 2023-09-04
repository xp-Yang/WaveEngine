#include "Scene.hpp"
#include "MyCube.hpp"
#include "MySphere.hpp"
#include "MyGround.hpp"
#include "MyModel.hpp"
#include "ECS/World.hpp"
#include "ECS/Components.hpp"

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

void Scene::init() {
    Skybox* skybox = new Skybox();
    MyLight* light = new MyLight();
    MyCube* cube = new MyCube();
    MySphere* sphere = new MySphere();
    MyGround* ground = new MyGround();
    Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
    Model* yoko = new Model("resource/model/yoko/008.obj");
    
    insert_object("cube", cube);
    insert_object("sphere", sphere);
    insert_object("ground", ground);
    insert_object("nanosuit", nanosuit);
    insert_object("yoko", yoko);
    set_skybox(skybox);
    set_light(light);

    // TODO 这些需要被scene管理吗，参考filament
	auto& world = ecs::World::get();

	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	world.addComponent<ecs::MeshComponent>(skybox_entity).meshes = get_skybox().get_meshes();
	world.addComponent<ecs::SkyboxComponent>(skybox_entity).texture = get_skybox().get_texture_id();
	Material skybox_material;
	skybox_material.shader = new Shader("resource/shader/skybox.vs", "resource/shader/skybox.fs");
	skybox_material.diffuse_map = skybox->get_texture_id();
	skybox_material.diffuse_map_path = "resource/images/skybox/test.jpg";
    world.addComponent<ecs::MaterialComponent>(skybox_entity).materials.push_back(skybox_material);

	auto light_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(light_entity).name = "light";
    world.addComponent<ecs::LightComponent>(light_entity);
	auto& light_transform = world.addComponent<ecs::TransformComponent>(light_entity);
    light_transform.translation = { 12.0f, 12.0f, 0.0f };
    light_transform.scale = glm::vec3(0.2f);
	auto& light_mesh = world.addComponent<ecs::MeshComponent>(light_entity);
	light_mesh.meshes = get_light().get_meshes();
	Material light_material;
	light_material.shader = new Shader("resource/shader/light.vs", "resource/shader/light.fs");
    world.addComponent<ecs::MaterialComponent>(light_entity).materials.push_back(light_material);

	auto cube_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(cube_entity).name = "cube";
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 0.0f, 1.0f, 0.0f };
	world.addComponent<ecs::MeshComponent>(cube_entity).meshes = object("cube")->get_meshes();
	Material cube_material;
    cube_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	cube_material.set_diffuse_map("resource/images/default_white_map.png");
	cube_material.set_specular_map("resource/images/cube_specular.png");
    world.addComponent<ecs::MaterialComponent>(cube_entity).materials.push_back(cube_material);

	auto sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(sphere_entity).name = "sphere";
	auto& sphere_transform = world.addComponent<ecs::TransformComponent>(sphere_entity);
    sphere_transform.translation = { 1.0f, 2.0f, 2.0f };
	world.addComponent<ecs::MeshComponent>(sphere_entity).meshes = object("sphere")->get_meshes();
	Material sphere_material;
	sphere_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	sphere_material.set_diffuse_map("resource/images/default_white_map.png");
	sphere_material.set_specular_map("resource/images/default_white_map.png");
    world.addComponent<ecs::MaterialComponent>(sphere_entity).materials.push_back(sphere_material);

    auto ground_entity = world.create_entity();
    world.addComponent<ecs::NameComponent>(ground_entity).name = "ground";
    auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = glm::vec3(20.0f);
    world.addComponent<ecs::MeshComponent>(ground_entity).meshes = object("ground")->get_meshes();
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.set_diffuse_map("resource/images/default_white_map.png");
    ground_material.set_specular_map("resource/images/default_white_map.png");
    world.addComponent<ecs::MaterialComponent>(ground_entity).materials.push_back(ground_material);

	auto nanosuit_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(nanosuit_entity).name = "nanosuit";
	auto& nanosuit_transform = world.addComponent<ecs::TransformComponent>(nanosuit_entity);
    nanosuit_transform.translation = {5.0f, 0.0f, 0.0f};
	nanosuit_transform.scale = glm::vec3(0.4f);
	world.addComponent<ecs::MeshComponent>(nanosuit_entity).meshes = object("nanosuit")->get_meshes();
	Shader * nanosuit_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	for (int i = 0; i < nanosuit->get_materials().size(); i++) {
		nanosuit->material(i).shader = nanosuit_shader;
	}
	world.addComponent<ecs::MaterialComponent>(nanosuit_entity).materials = object("nanosuit")->get_materials();

	auto yoko_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(yoko_entity).name = "yoko";
	auto& yoko_transform = world.addComponent<ecs::TransformComponent>(yoko_entity);
	yoko_transform.translation = { -5.0f, 0.0f, 0.0f };
	yoko_transform.scale = glm::vec3(0.25f);
	world.addComponent<ecs::MeshComponent>(yoko_entity).meshes = object("yoko")->get_meshes();
	Shader* yoko_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	for (int i = 0; i < yoko->get_materials().size(); i++) {
		yoko->material(i).shader = yoko_shader;
	}
	world.addComponent<ecs::MaterialComponent>(yoko_entity).materials = object("yoko")->get_materials();

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}