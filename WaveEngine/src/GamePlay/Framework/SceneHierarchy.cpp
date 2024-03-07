#include "SceneHierarchy.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "ResourceManager/MyModel.hpp"
#include "ResourceManager/Texture.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Application_impl.hpp"

SceneHierarchy::SceneHierarchy() {
	init();
}

void SceneHierarchy::init() {
	m_root_node = new GameObjectNode(nullptr, nullptr);

	std::string resource_dir = Application::resourceDirectory();

    // TODO 这些需要被SceneHierarchy管理吗，参考filament
	auto& world = ecs::World::get();

	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "Skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	auto& skybox_renderable = world.addComponent<ecs::RenderableComponent>(skybox_entity);
	ecs::Primitive skybox_primitive;
	skybox_primitive.mesh = Mesh::create_cube_mesh();
	std::vector<std::string> faces
	{
		resource_dir + "/images/skybox/right.jpg",
		resource_dir + "/images/skybox/left.jpg",
		resource_dir + "/images/skybox/top.jpg",
		resource_dir + "/images/skybox/bottom.jpg",
		resource_dir + "/images/skybox/front.jpg",
		resource_dir + "/images/skybox/back.jpg"
	};
	// TODO 是否可以放到material中
	// TODO component size crash
	world.addComponent<ecs::SkyboxComponent>(skybox_entity).texture = Texture::generate_cube_map(faces);
	Material skybox_material;
	skybox_material.shader = new Shader(resource_dir + "/shader/skybox.vs", resource_dir + "/shader/skybox.fs");
	skybox_primitive.material = skybox_material;
	skybox_renderable.setPrimitives({ skybox_primitive });


	auto dir_light_entity = world.create_entity();
	auto directional_light_obj = new GameObject(dir_light_entity);
	auto directional_light_node = new GameObjectNode(m_root_node, directional_light_obj);
	world.addComponent<ecs::NameComponent>(dir_light_entity).name = "Directional Light";
	auto& dir_light_properties = world.addComponent<ecs::DirectionalLightComponent>(dir_light_entity);
	dir_light_properties.luminousColor = { 0.02f, 0.02f, 0.02f, 1.0f };
	auto& dir_light_transform = world.addComponent<ecs::TransformComponent>(dir_light_entity);
	dir_light_transform.translation = { -15.0f, 30.0f, -15.0f };
	dir_light_properties.direction = -dir_light_transform.translation;


	static const int POINT_LIGHT_COUNT = 3;
	static Shader* point_light_shader = new Shader(resource_dir + "/shader/light.vs", resource_dir + "/shader/light.fs");
	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		auto point_light_entity = world.create_entity();
		auto point_light_obj = new GameObject(point_light_entity);
		auto point_light_node = new GameObjectNode(m_root_node, point_light_obj);
		world.addComponent<ecs::NameComponent>(point_light_entity).name = std::string("Point Light ") + std::to_string(i);
		auto& point_light_transform = world.addComponent<ecs::TransformComponent>(point_light_entity);
		double r1 = random(-15.0f, 15.0f);
		double r2 = random(3.0f, 10.0f);
		double r3 = random(-15.0f, 15.0f);
		point_light_transform.translation = { r1, r2, r3 };
		point_light_transform.scale = Vec3(random(0.1f, 0.3f));
		auto& point_light_renderable = world.addComponent<ecs::RenderableComponent>(point_light_entity);
		auto& point_light_properties = world.addComponent<ecs::PointLightComponent>(point_light_entity);
		point_light_properties.radius = (point_light_transform.scale[0] / 0.1f) * 20.0f;
		point_light_properties.luminousColor = { randomUnit(), randomUnit(), randomUnit(), 1.0f};
		ecs::Primitive point_light_primitive;
		point_light_primitive.mesh = Mesh::create_icosphere_mesh(5);
		Material point_light_material;
		point_light_material.shader = point_light_shader;
		point_light_primitive.material = point_light_material;
		point_light_renderable.setPrimitives({ point_light_primitive });
	}

	auto cube_entity = world.create_entity();
	auto cube_object = new GameObject(cube_entity);
	auto cube_node = new GameObjectNode(m_root_node, cube_object);
	world.addComponent<ecs::NameComponent>(cube_entity).name = "Cube";
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 0.0f, 0.5f, -10.0f };
	auto& cube_renderable = world.addComponent<ecs::RenderableComponent>(cube_entity);
	ecs::Primitive cube_primitive;
	cube_primitive.mesh = Mesh::create_cube_mesh();
	Material cube_material;
    cube_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	cube_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	cube_material.set_specular_map(resource_dir + "/images/cube_specular.png");
	cube_primitive.material = cube_material;
	cube_renderable.setPrimitives({ cube_primitive });
    world.addComponent<ecs::ExplosionComponent>(cube_entity);

	auto sphere_entity = world.create_entity();
	auto sphere_object = new GameObject(sphere_entity);
	auto sphere_node = new GameObjectNode(m_root_node, sphere_object);
	world.addComponent<ecs::NameComponent>(sphere_entity).name = "Sphere";
	auto& sphere_transform = world.addComponent<ecs::TransformComponent>(sphere_entity);
    sphere_transform.translation = { 5.0f, 1.0f, 5.0f };
	auto& sphere_renderable = world.addComponent<ecs::RenderableComponent>(sphere_entity);
	ecs::Primitive sphere_primitive;
	sphere_primitive.mesh = Mesh::create_icosphere_mesh(5);
	Material sphere_material;
	sphere_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	sphere_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	sphere_material.set_specular_map(resource_dir + "/images/pure_white_map.png");
	sphere_primitive.material = sphere_material;
	sphere_renderable.setPrimitives({ sphere_primitive });
	world.addComponent<ecs::ExplosionComponent>(sphere_entity);

	//TODO shader在延迟渲染中没用到。shader是否不应该在这里创建
    auto ground_entity = world.create_entity();
	auto ground_object = new GameObject(ground_entity);
	auto ground_node = new GameObjectNode(m_root_node, ground_object);
    world.addComponent<ecs::NameComponent>(ground_entity).name = "Grid";
	world.addComponent<ecs::BaseGridGroundComponent>(ground_entity);
    auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = Vec3(1.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	ecs::Primitive ground_primitive;
	ground_primitive.mesh = Mesh::create_ground_mesh();
    Material ground_material;
    ground_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/wireframe.gs", resource_dir + "/shader/wireframe.fs");
    ground_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
    ground_material.set_specular_map(resource_dir + "/images/pure_white_map.png");
	ground_primitive.material = ground_material;
	ground_renderable.setPrimitives({ ground_primitive });

	Model* nanosuit = new Model(resource_dir + "/model/nanosuit/nanosuit.obj");
	auto nanosuit_entity = world.create_entity();
	auto nanosuit_object = new GameObject(nanosuit_entity);
	auto nanosuit_node = new GameObjectNode(m_root_node, nanosuit_object);
	world.addComponent<ecs::NameComponent>(nanosuit_entity).name = "Nanosuit";
	auto& nanosuit_transform = world.addComponent<ecs::TransformComponent>(nanosuit_entity);
	nanosuit_transform.translation = { 5.0f, 0.0f, 0.0f };
	nanosuit_transform.scale = Vec3(0.4f);
	auto& nanosuit_renderable = world.addComponent<ecs::RenderableComponent>(nanosuit_entity);
	std::vector<ecs::Primitive> nanosuit_primitives;
	Shader* nanosuit_shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	for (int i = 0; i < nanosuit->get_datas().size(); i++) {
		ecs::Primitive primitive;
		primitive.mesh = nanosuit->get_datas().at(i).mesh;
		primitive.material = nanosuit->get_datas().at(i).material;
		primitive.material.shader = nanosuit_shader;
		nanosuit_primitives.push_back(primitive);
	}
	nanosuit_renderable.setPrimitives(nanosuit_primitives);
	world.addComponent<ecs::ExplosionComponent>(nanosuit_entity);

	//Model* yoko = new Model(resource_dir + "/model/yoko/008.obj");
	//auto yoko_entity = world.create_entity();
	//world.addComponent<ecs::NameComponent>(yoko_entity).name = "Yoko";
	//auto& yoko_transform = world.addComponent<ecs::TransformComponent>(yoko_entity);
	//yoko_transform.translation = { -5.0f, 0.0f, 0.0f };
	//yoko_transform.scale = Vec3(0.25f);
	//auto& yoko_renderable = world.addComponent<ecs::RenderableComponent>(yoko_entity);
	//std::vector<ecs::Primitive> yoko_primitives;
	//Shader* yoko_shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	//for (int i = 0; i < yoko->get_datas().size(); i++) {
	//	ecs::Primitive primitive;
	//	primitive.mesh = yoko->get_datas().at(i).mesh;
	//	primitive.material = yoko->get_datas().at(i).material;
	//	primitive.material.shader = yoko_shader;
	//	yoko_primitives.push_back(primitive);
	//}
	//yoko_renderable.setPrimitives(yoko_primitives);
	//world.addComponent<ecs::ExplosionComponent>(yoko_entity);

	Model* bunny = new Model(resource_dir + "/model/bunny.obj");
	auto bunny_entity = world.create_entity();
	auto bunny_object = new GameObject(bunny_entity);
	auto bunny_node = new GameObjectNode(m_root_node, bunny_object);
	world.addComponent<ecs::NameComponent>(bunny_entity).name = "Bunny";
	auto& bunny_transform = world.addComponent<ecs::TransformComponent>(bunny_entity);
	bunny_transform.translation = { -5.0f, 0.0f, 0.0f };
	bunny_transform.scale = Vec3(25.0f);
	auto& bunny_renderable = world.addComponent<ecs::RenderableComponent>(bunny_entity);
	std::vector<ecs::Primitive> bunny_primitives;
	Shader* bunny_shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	for (int i = 0; i < bunny->get_datas().size(); i++) {
		ecs::Primitive primitive;
		primitive.mesh = bunny->get_datas().at(i).mesh;
		primitive.material = bunny->get_datas().at(i).material;
		primitive.material.shader = bunny_shader;
		primitive.material = sphere_material;
		bunny_primitives.push_back(primitive);
	}
	bunny_renderable.setPrimitives(bunny_primitives);
	world.addComponent<ecs::ExplosionComponent>(bunny_entity);

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "Main Camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}