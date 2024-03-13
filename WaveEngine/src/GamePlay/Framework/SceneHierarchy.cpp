#include "SceneHierarchy.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "ResourceManager/MyModel.hpp"
#include "ResourceManager/Texture.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Application_impl.hpp"

SceneHierarchy::SceneHierarchy() {
	init();
}

GameObject* SceneHierarchy::loadModal(const std::string& filepath)
{
	std::string resource_dir = Application::resourceDirectory();

	auto& world = ecs::World::get();

	Model* model = new Model(filepath);
	auto entity = world.create_entity();
	std::string name = filepath.substr(filepath.find_last_of("/\\") + 1, filepath.find_last_of('.') - filepath.find_last_of("/\\") - 1);
	world.addComponent<ecs::NameComponent>(entity).name = name;
	world.addComponent<ecs::TransformComponent>(entity);
	//world.addComponent<ecs::ExplosionComponent>(bunny_entity);
	auto& renderable = world.addComponent<ecs::RenderableComponent>(entity);
	std::vector<ecs::Primitive> primitives;
	for (int i = 0; i < model->get_datas().size(); i++) {
		ecs::Primitive primitive;
		primitive.mesh = model->get_datas().at(i).mesh;
		if (model->get_datas().at(i).material.diffuse_map == 0 && model->get_datas().at(i).material.specular_map == 0) {
			Material material;
			material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
			material.set_specular_map(resource_dir + "/images/pure_white_map.png");
			primitive.material = material;
		}
		else
			primitive.material = model->get_datas().at(i).material;
		Shader* shader = new Shader(resource_dir + "/shader/pbr.vs", resource_dir + "/shader/pbr.fs");
		primitive.material.shader = shader;
		primitives.push_back(primitive);
	}
	renderable.setPrimitives(primitives);

	return new GameObject(m_root_object, entity);
}

void SceneHierarchy::addPointLight()
{
	std::string resource_dir = Application::resourceDirectory();
	static Shader* point_light_shader = new Shader(resource_dir + "/shader/light.vs", resource_dir + "/shader/light.fs");

	auto& world = ecs::World::get();

	auto point_light_entity = world.create_entity();
	auto point_light_node = new GameObject(m_root_point_light_object, point_light_entity);
	world.addComponent<ecs::NameComponent>(point_light_entity).name = std::string("Point Light ") + std::to_string(m_point_light_count);
	auto& point_light_transform = world.addComponent<ecs::TransformComponent>(point_light_entity);
	double r1 = random(-15.0f, 15.0f);
	double r2 = random(3.0f, 10.0f);
	double r3 = random(-15.0f, 15.0f);
	point_light_transform.translation = { r1, r2, r3 };
	point_light_transform.scale = Vec3(random(0.1f, 0.3f));
	auto& point_light_renderable = world.addComponent<ecs::RenderableComponent>(point_light_entity);
	auto& point_light_properties = world.addComponent<ecs::PointLightComponent>(point_light_entity);
	point_light_properties.radius = (point_light_transform.scale[0] / 0.1f) * 20.0f;
	point_light_properties.luminousColor = { randomUnit(), randomUnit(), randomUnit(), 1.0f };
	ecs::Primitive point_light_primitive;
	point_light_primitive.mesh = Mesh::create_icosphere_mesh(5);
	Material point_light_material;
	point_light_material.shader = point_light_shader;
	point_light_primitive.material = point_light_material;
	point_light_renderable.setPrimitives({ point_light_primitive });

	m_point_light_count++;
}

void SceneHierarchy::addCube()
{
	std::string resource_dir = Application::resourceDirectory();

	auto& world = ecs::World::get();

	auto cube_entity = world.create_entity();
	auto cube_node = new GameObject(m_root_cube_object, cube_entity);
	world.addComponent<ecs::NameComponent>(cube_entity).name = std::string("Cube") + std::to_string(m_test_cube_count);
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 1.5f * (m_test_cube_count % 6), 0.5f + 1.5f * (m_test_cube_count / 6), -10.0f };
	auto& cube_renderable = world.addComponent<ecs::RenderableComponent>(cube_entity);
	ecs::Primitive cube_primitive;
	cube_primitive.mesh = Mesh::create_cube_mesh();
	Material cube_material;
	//cube_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	cube_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	cube_material.set_specular_map(resource_dir + "/images/cube_specular.png");
	cube_material.shader = new Shader(resource_dir + "/shader/pbr.vs", resource_dir + "/shader/pbr.fs");
	cube_material.albedo = Vec3(1.0f);
	cube_material.metallic = 1.0;
	cube_material.roughness = 0.5;
	cube_material.ao = 0.0;
	cube_primitive.material = cube_material;
	cube_renderable.setPrimitives({ cube_primitive });
	//world.addComponent<ecs::ExplosionComponent>(cube_entity);

	m_test_cube_count++;
}

void SceneHierarchy::addSphere()
{
	std::string resource_dir = Application::resourceDirectory();

	auto& world = ecs::World::get();

	auto sphere_entity = world.create_entity();
	auto sphere_node = new GameObject(m_root_sphere_object, sphere_entity);
	world.addComponent<ecs::NameComponent>(sphere_entity).name = std::string("Sphere") + std::to_string(m_test_sphere_count);
	auto& sphere_transform = world.addComponent<ecs::TransformComponent>(sphere_entity);
	sphere_transform.translation = { 2.5f * (m_test_sphere_count % 5), 1.0f + 2.5f * (m_test_sphere_count / 5), 0 };
	auto& sphere_renderable = world.addComponent<ecs::RenderableComponent>(sphere_entity);
	ecs::Primitive sphere_primitive;
	sphere_primitive.mesh = Mesh::create_icosphere_mesh(5);
	Material sphere_material;
	//sphere_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/modelFowardRendering.fs");
	sphere_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	sphere_material.set_specular_map(resource_dir + "/images/pure_white_map.png");
	sphere_material.shader = new Shader(resource_dir + "/shader/pbr.vs", resource_dir + "/shader/pbr.fs");
	sphere_material.albedo = Vec3(1.0f, 1.0f, 1.0f);
	sphere_material.metallic = 1.0;
	sphere_material.roughness = (1.0f / 25) * m_test_sphere_count;
	sphere_material.ao = 0.01;
	sphere_primitive.material = sphere_material;
	sphere_renderable.setPrimitives({ sphere_primitive });
	//world.addComponent<ecs::ExplosionComponent>(sphere_entity);

	m_test_sphere_count++;
}

void SceneHierarchy::init() {
	std::string resource_dir = Application::resourceDirectory();

    // TODO 这些需要被SceneHierarchy管理吗，参考filament
	auto& world = ecs::World::get();

	auto root_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_entity).name = "Root";
	m_root_object = new GameObject(nullptr, root_entity);

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
	auto directional_light_node = new GameObject(m_root_object, dir_light_entity);
	world.addComponent<ecs::NameComponent>(dir_light_entity).name = "Directional Light";
	auto& dir_light_properties = world.addComponent<ecs::DirectionalLightComponent>(dir_light_entity);
	dir_light_properties.luminousColor = { 2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f, 1.0f };
	auto& dir_light_transform = world.addComponent<ecs::TransformComponent>(dir_light_entity);
	dir_light_transform.translation = { -15.0f, 30.0f, -15.0f };
	dir_light_properties.direction = -dir_light_transform.translation;

	auto root_point_lights_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_point_lights_entity).name = "Point Lights";
	m_root_point_light_object = new GameObject(m_root_object, root_point_lights_entity);
	for (int i = 0; i < 3; i++) {
		addPointLight();
	}

	auto root_cube_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_cube_entity).name = "Cubes";
	m_root_cube_object = new GameObject(m_root_object, root_cube_entity);
	for (int i = 0; i < 36; i++) {
		addCube();
	}

	auto root_sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_sphere_entity).name = "Spheres";
	m_root_sphere_object = new GameObject(m_root_object, root_sphere_entity);
	for (int i = 0; i < 25; i++) {
		addSphere();
	}

	//TODO shader在延迟渲染中没用到。shader是否不应该在这里创建
    auto ground_entity = world.create_entity();
	auto ground_node = new GameObject(m_root_object, ground_entity);
    world.addComponent<ecs::NameComponent>(ground_entity).name = "Gound";
	world.addComponent<ecs::BaseGridGroundComponent>(ground_entity);
    auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = Vec3(1.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	ecs::Primitive ground_primitive;
	ground_primitive.mesh = Mesh::create_ground_mesh();
    Material ground_material;
    //ground_material.shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/wireframe.gs", resource_dir + "/shader/wireframe.fs");
    ground_material.shader = Shader::getShader(ShaderType::CheckerboardShader);
    ground_material.set_diffuse_map(resource_dir + "/images/pure_white_map.png");
    ground_material.set_specular_map(resource_dir + "/images/pure_white_map.png");
	ground_primitive.material = ground_material;
	ground_renderable.setPrimitives({ ground_primitive });

	loadModal(resource_dir + "/model/nanosuit/nanosuit.obj");

	loadModal(resource_dir + "/model/bunny.obj");

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "Main Camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}