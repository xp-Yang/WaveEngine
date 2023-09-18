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
    // TODO 这些需要被scene管理吗，参考filament
	auto& world = ecs::World::get();

	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	auto& skybox_renderable = world.addComponent<ecs::RenderableComponent>(skybox_entity);
	ecs::Primitive skybox_primitive;
	skybox_primitive.mesh = Mesh::create_cube_mesh();
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
	skybox_primitive.material = skybox_material;
	skybox_renderable.setPrimitives({ skybox_primitive });

	auto light_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(light_entity).name = "light";
    world.addComponent<ecs::LightComponent>(light_entity);
	auto& light_transform = world.addComponent<ecs::TransformComponent>(light_entity);
    light_transform.translation = { 12.0f, 12.0f, 0.0f };
    light_transform.scale = glm::vec3(0.5f);
	auto& light_renderable = world.addComponent<ecs::RenderableComponent>(light_entity);
	ecs::Primitive light_primitive;
	light_primitive.mesh = Mesh::create_cube_mesh();
	Material light_material;
	light_material.shader = new Shader("resource/shader/light.vs", "resource/shader/light.fs");
	light_primitive.material = light_material;
	light_renderable.setPrimitives({ light_primitive });

	auto cube_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(cube_entity).name = "cube";
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 0.0f, 1.0f, 0.0f };
	auto& cube_renderable = world.addComponent<ecs::RenderableComponent>(cube_entity);
	ecs::Primitive cube_primitive;
	cube_primitive.mesh = Mesh::create_cube_mesh();
	Material cube_material;
	cube_material.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
    cube_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	cube_material.set_diffuse_map("resource/images/default_white_map.png");
	cube_material.set_specular_map("resource/images/cube_specular.png");
	cube_primitive.material = cube_material;
	cube_renderable.setPrimitives({ cube_primitive });
    world.addComponent<ecs::ExplosionComponent>(cube_entity);

	auto sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(sphere_entity).name = "sphere";
	auto& sphere_transform = world.addComponent<ecs::TransformComponent>(sphere_entity);
    sphere_transform.translation = { 1.0f, 2.0f, 2.0f };
	auto& sphere_renderable = world.addComponent<ecs::RenderableComponent>(sphere_entity);
	ecs::Primitive sphere_primitive;
	sphere_primitive.mesh = Mesh::create_icosphere_mesh(8);
	Material sphere_material;
	sphere_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	sphere_material.set_diffuse_map("resource/images/default_white_map.png");
	sphere_material.set_specular_map("resource/images/default_white_map.png");
	sphere_primitive.material = sphere_material;
	sphere_renderable.setPrimitives({ sphere_primitive });
	world.addComponent<ecs::ExplosionComponent>(sphere_entity);

    auto ground_entity = world.create_entity();
    world.addComponent<ecs::NameComponent>(ground_entity).name = "ground";
    auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = glm::vec3(200.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	ecs::Primitive ground_primitive;
	ground_primitive.mesh = Mesh::create_quad_mesh();
    Material ground_material;
    ground_material.shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
    ground_material.set_diffuse_map("resource/images/grid.png");
    ground_material.set_specular_map("resource/images/grid.png");
	ground_primitive.material = ground_material;
	ground_renderable.setPrimitives({ ground_primitive });

	Model* nanosuit = new Model("resource/model/nanosuit/nanosuit.obj");
	auto nanosuit_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(nanosuit_entity).name = "nanosuit";
	auto& nanosuit_transform = world.addComponent<ecs::TransformComponent>(nanosuit_entity);
	nanosuit_transform.translation = { 5.0f, 0.0f, 0.0f };
	nanosuit_transform.scale = glm::vec3(0.4f);
	auto& nanosuit_renderable = world.addComponent<ecs::RenderableComponent>(nanosuit_entity);
	std::vector<ecs::Primitive> nanosuit_primitives;
	Shader* nanosuit_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	for (int i = 0; i < nanosuit->get_datas().size(); i++) {
		ecs::Primitive primitive;
		primitive.mesh = nanosuit->get_datas().at(i).mesh;
		primitive.material = nanosuit->get_datas().at(i).material;
		primitive.material.shader = nanosuit_shader;
		nanosuit_primitives.push_back(primitive);
	}
	nanosuit_renderable.setPrimitives({ nanosuit_primitives });
	world.addComponent<ecs::ExplosionComponent>(nanosuit_entity);

	Model* yoko = new Model("resource/model/yoko/008.obj");
	auto yoko_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(yoko_entity).name = "yoko";
	auto& yoko_transform = world.addComponent<ecs::TransformComponent>(yoko_entity);
	yoko_transform.translation = { -5.0f, 0.0f, 0.0f };
	yoko_transform.scale = glm::vec3(0.25f);
	auto& yoko_renderable = world.addComponent<ecs::RenderableComponent>(yoko_entity);
	std::vector<ecs::Primitive> yoko_primitives;
	Shader* yoko_shader = new Shader("resource/shader/model.vs", "resource/shader/model.fs", "resource/shader/model.gs");
	for (int i = 0; i < yoko->get_datas().size(); i++) {
		ecs::Primitive primitive;
		primitive.mesh = yoko->get_datas().at(i).mesh;
		primitive.material = yoko->get_datas().at(i).material;
		primitive.material.shader = yoko_shader;
		yoko_primitives.push_back(primitive);
	}
	yoko_renderable.setPrimitives({ yoko_primitives });
	world.addComponent<ecs::ExplosionComponent>(yoko_entity);

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}