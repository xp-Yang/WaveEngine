#include "Scene.hpp"
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
    Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
    Model* yoko = new Model("resource/model/yoko/008.obj");
    
    insert_object("nanosuit", nanosuit);
    insert_object("yoko", yoko);

    // TODO 这些需要被scene管理吗，参考filament
	auto& world = ecs::World::get();

	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	world.addComponent<ecs::MeshComponent>(skybox_entity).meshes.push_back(Mesh::create_cube_mesh());
	std::vector<std::string> faces
	{
		"resource/images/skybox/right.jpg",
		"resource/images/skybox/left.jpg",
		"resource/images/skybox/top.jpg",
		"resource/images/skybox/bottom.jpg",
		"resource/images/skybox/front.jpg",
		"resource/images/skybox/back.jpg"
	};
	// TODO 是否可以放到material中
	world.addComponent<ecs::SkyboxComponent>(skybox_entity).texture = Texture::generate_cube_map(faces);
	Material skybox_material;
	skybox_material.shader = new Shader("resource/shader/skybox.vs", "resource/shader/skybox.fs");
    world.addComponent<ecs::MaterialComponent>(skybox_entity).materials.push_back(skybox_material);

	auto light_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(light_entity).name = "light";
    world.addComponent<ecs::LightComponent>(light_entity);
	auto& light_transform = world.addComponent<ecs::TransformComponent>(light_entity);
    light_transform.translation = { 12.0f, 12.0f, 0.0f };
    light_transform.scale = glm::vec3(0.5f);
	world.addComponent<ecs::MeshComponent>(light_entity).meshes.push_back(Mesh::create_cube_mesh());
	Material light_material;
	light_material.shader = new Shader("resource/shader/light.vs", "resource/shader/light.fs");
    world.addComponent<ecs::MaterialComponent>(light_entity).materials.push_back(light_material);

	auto cube_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(cube_entity).name = "cube";
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 0.0f, 1.0f, 0.0f };
	world.addComponent<ecs::MeshComponent>(cube_entity).meshes.push_back(Mesh::create_cube_mesh());
	Material cube_material;
	cube_material.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
    cube_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	cube_material.set_diffuse_map("resource/images/default_white_map.png");
	cube_material.set_specular_map("resource/images/cube_specular.png");
    world.addComponent<ecs::MaterialComponent>(cube_entity).materials.push_back(cube_material);
    world.addComponent<ecs::TransparentComponent>(cube_entity);
    world.addComponent<ecs::ExplosionComponent>(cube_entity);

	auto sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(sphere_entity).name = "sphere";
	auto& sphere_transform = world.addComponent<ecs::TransformComponent>(sphere_entity);
    sphere_transform.translation = { 1.0f, 2.0f, 2.0f };
	world.addComponent<ecs::MeshComponent>(sphere_entity).meshes.push_back(Mesh::create_icosphere_mesh(8));
	Material sphere_material;
	sphere_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	sphere_material.set_diffuse_map("resource/images/default_white_map.png");
	sphere_material.set_specular_map("resource/images/default_white_map.png");
    world.addComponent<ecs::MaterialComponent>(sphere_entity).materials.push_back(sphere_material);
	world.addComponent<ecs::TransparentComponent>(sphere_entity);
	world.addComponent<ecs::ExplosionComponent>(sphere_entity);

    auto ground_entity = world.create_entity();
    world.addComponent<ecs::NameComponent>(ground_entity).name = "ground";
    auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = glm::vec3(200.0f);
    world.addComponent<ecs::MeshComponent>(ground_entity).meshes.push_back(Mesh::create_quad_mesh());
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.set_diffuse_map("resource/images/grid.png");
    ground_material.set_specular_map("resource/images/grid.png");
	// 一种纹理网格， 一种自己绘制的网格
    world.addComponent<ecs::MaterialComponent>(ground_entity).materials.push_back(ground_material);
	world.addComponent<ecs::TransparentComponent>(ground_entity);

	auto nanosuit_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(nanosuit_entity).name = "nanosuit";
	auto& nanosuit_transform = world.addComponent<ecs::TransformComponent>(nanosuit_entity);
	nanosuit_transform.translation = { 5.0f, 0.0f, 0.0f };
	nanosuit_transform.scale = glm::vec3(0.4f);
	world.addComponent<ecs::MeshComponent>(nanosuit_entity).meshes = object("nanosuit")->get_meshes();
	Shader * nanosuit_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	for (int i = 0; i < nanosuit->get_materials().size(); i++) {
		nanosuit->material(i).shader = nanosuit_shader;
	}
	world.addComponent<ecs::MaterialComponent>(nanosuit_entity).materials = object("nanosuit")->get_materials();
	world.addComponent<ecs::TransparentComponent>(nanosuit_entity);
	world.addComponent<ecs::ExplosionComponent>(nanosuit_entity);

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
	world.addComponent<ecs::TransparentComponent>(yoko_entity);
	world.addComponent<ecs::ExplosionComponent>(yoko_entity);

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}