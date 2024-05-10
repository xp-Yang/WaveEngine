#include "SceneHierarchy.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#include "ResourceManager/ObjImporter.hpp"
#include "ResourceManager/Texture.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Core/Logger/Logger.hpp"
#include "Application_impl.hpp"
#include "Core/Meta/Serializer.hpp"

static const std::string resource_dir = Application::resourceDirectory();
static const std::string asset_dir = Application::assetDirectory();

SceneHierarchy::SceneHierarchy()
	: world(ecs::World::get())
{
	init();
}

SceneHierarchy::~SceneHierarchy()
{
}

ecs::Object* SceneHierarchy::loadModel(const std::string& filepath)
{
	Logger::Logger::get().info("SceneHierarchy::loadModel({})", filepath);
	std::shared_ptr<Asset::Mesh> obj_mesh = Asset::ObjImporter::get().load(filepath);
	if (obj_mesh->sub_meshes.empty()) {
		Logger::Logger::get().error("Model datas is empty. File loading fails. Please check if the filepath is all English.");
		return nullptr;
	}
	auto entity = world.create_entity();
	std::string name = filepath.substr(filepath.find_last_of("/\\") + 1, filepath.find_last_of('.') - filepath.find_last_of("/\\") - 1);
	world.addComponent<ecs::NameComponent>(entity).name = name;
	world.addComponent<ecs::TransformComponent>(entity);
	//world.addComponent<ecs::ExplosionComponent>(entity);
	auto& renderable = world.addComponent<ecs::RenderableComponent>(entity);
	renderable.sub_meshes = std::move(obj_mesh->sub_meshes);

	auto res = new ecs::Object(m_root_object, entity);
	return res;
}

void SceneHierarchy::addObject(ecs::Object* obj, ecs::Object* parent)
{
	Logger::Logger::get().trace("SceneHierarchy::addObject(entity = {})", obj->entity().getId());

	if (ecs::World::get().hasComponent<ecs::PointLightComponent>(obj->entity()))
		m_point_light_count++;
	parent ? parent->append(obj) : m_root_object->append(obj);
}

void SceneHierarchy::removeObject(const ecs::Entity& entity)
{
	Logger::Logger::get().trace("SceneHierarchy::removeObject(entity = {})", entity.getId());

	if (ecs::World::get().hasComponent<ecs::PointLightComponent>(entity))
		m_point_light_count--;
	m_root_object->remove(entity);
}

void SceneHierarchy::addPointLight()
{
	if (pointLightCount() >= SceneHierarchy::maxPointLightCount())
		return;

	Logger::Logger::get().info("SceneHierarchy::addPointLight(), count:{}", m_point_light_count + 1);

	static Shader* point_light_shader = new Shader(resource_dir + "/shader/light.vs", resource_dir + "/shader/light.fs");

	auto point_light_entity = world.create_entity();
	auto point_light_node = new ecs::Object(m_root_point_light_object, point_light_entity);
	world.addComponent<ecs::NameComponent>(point_light_entity).name = std::string("Point Light ") + std::to_string(m_point_light_count);
	auto& point_light_transform = world.addComponent<ecs::TransformComponent>(point_light_entity);
	double r1 = random(-15.0f, 15.0f);
	double r2 = random(3.0f, 10.0f);
	double r3 = random(-15.0f, 15.0f);
	point_light_transform.translation = { r1, r2, r3 };
	point_light_transform.scale = Vec3(random(0.1f, 0.3f));
	auto& point_light_renderable = world.addComponent<ecs::RenderableComponent>(point_light_entity);
	auto& point_light_properties = world.addComponent<ecs::PointLightComponent>(point_light_entity);
	point_light_properties.radius = (point_light_transform.scale[0]) * 100.0f;
	point_light_properties.luminousColor = m_point_light_count == 0 ? Color4(2.0f, 2.0f, 2.0f, 1.0f) : Color4{ randomUnit(), randomUnit(), randomUnit(), 1.0f };
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_icosphere_mesh(4);
	sub_mesh.material = std::make_shared<Asset::Material>();
	point_light_renderable.sub_meshes.push_back(sub_mesh);

	m_point_light_count++;
}

void SceneHierarchy::addCube()
{
	Logger::Logger::get().info("SceneHierarchy::addCube(), count:{}", m_test_cube_count + 1);

	auto cube_entity = world.create_entity();
	auto cube_node = new ecs::Object(m_root_cube_object, cube_entity);
	world.addComponent<ecs::NameComponent>(cube_entity).name = std::string("Cube") + std::to_string(m_test_cube_count);
	auto& cube_transform = world.addComponent<ecs::TransformComponent>(cube_entity);
	cube_transform.translation = { 1.5f * (m_test_cube_count % 6), 0.5f + 1.5f * (m_test_cube_count / 6), -10.0f };
	auto& cube_renderable = world.addComponent<ecs::RenderableComponent>(cube_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_cube_mesh();
	std::shared_ptr<Asset::Material> cube_material = std::make_shared<Asset::Material>();
	cube_material->albedo = Vec3(1.0f);
	cube_material->metallic = 1.0;
	cube_material->roughness = 0.5;
	cube_material->ao = 0.01;
	sub_mesh.material = cube_material;
	cube_renderable.sub_meshes.push_back(sub_mesh);
	//world.addComponent<ecs::ExplosionComponent>(cube_entity);

	m_test_cube_count++;
}


void SceneHierarchy::updateSpheresPosition()
{
	int one_sequence_max_num = 16;
	int max_cols = sqrt(one_sequence_max_num);
	int max_rows = max_cols;
	int sequences = m_test_sphere_count / one_sequence_max_num + 1;
	int cols = std::ceil(sqrt((m_test_sphere_count - 1) % one_sequence_max_num + 1));
	int rows = cols;

	float space = 3.0f;
	int i = 0;
	for (auto sphere_obj : m_root_sphere_object->children()) {
		auto& sphere_translation = world.getComponent<ecs::TransformComponent>(sphere_obj->entity())->translation;
		int sequence = i / one_sequence_max_num;
		if (sequence == sequences - 1) {
			int j = i - sequence * one_sequence_max_num;
			int col = j % cols;
			int row = (j / cols) % rows;
			sphere_translation = { space * col, 1.0f + space * row, space * sequence };
		}
		else {
			int col = i % max_cols;
			int row = (i / max_cols) % max_rows;
			sphere_translation = { space * col, 1.0f + space * row, space * sequence };
		}
		//float theta = (2 * Core::MathConstant::PI / 25.0f) * m_test_sphere_count;
		//sphere_translation = { 10.0f * cos(theta), 1.0f , 10.0f * sin(theta) };
		i++;
	}
}

void SceneHierarchy::addSphere()
{
	Logger::Logger::get().info("SceneHierarchy::addSphere(), count:{}", m_test_sphere_count + 1);

	auto sphere_entity = world.create_entity();
	auto sphere_node = new ecs::Object(m_root_sphere_object, sphere_entity);
	world.addComponent<ecs::NameComponent>(sphere_entity).name = std::string("Sphere") + std::to_string(m_test_sphere_count);
	world.addComponent<ecs::TransformComponent>(sphere_entity);

	auto& sphere_renderable = world.addComponent<ecs::RenderableComponent>(sphere_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_icosphere_mesh(4);
	std::shared_ptr<Asset::Material> sphere_material = std::make_shared<Asset::Material>();
	sphere_material->albedo = Vec3(1.0f, 1.0f, 1.0f);
	sphere_material->metallic = 1.0;
	sphere_material->roughness = (1.0f / 64) * (m_test_sphere_count + 1);
	sphere_material->ao = 0.01;
	sub_mesh.material = sphere_material;
	sphere_renderable.sub_meshes.push_back(sub_mesh);
	//world.addComponent<ecs::ExplosionComponent>(sphere_entity);

	m_test_sphere_count++;

	updateSpheresPosition();

	Meta::Serialization::Serializer::saveToJsonFile(asset_dir + "/sphere.json", world.getComponent<ecs::TransformComponent>(sphere_entity));
	//ecs::TransformComponent trans;
	//Meta::Serialization::Serializer::loadFromJsonFile(asset_dir + "/sphere.json", &trans);
	Meta::Serialization::Serializer::write(world.getComponent<ecs::TransformComponent>(sphere_entity));
}

void SceneHierarchy::removeSphere(size_t index)
{
	if (m_root_sphere_object->children().empty())
		return;

	Logger::Logger::get().info("SceneHierarchy::removeSphere(), count:{}", m_test_sphere_count - 1);

	if (index >= m_root_sphere_object->children().size())
		m_root_sphere_object->remove(m_root_sphere_object->children().back());
	else
		m_root_sphere_object->remove(m_root_sphere_object->children()[index]);

	m_test_sphere_count--;

	updateSpheresPosition();
}

void SceneHierarchy::initMainCamera()
{
	Logger::Logger::get().trace("SceneHierarchy::initMainCamera()");

	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "Main Camera";
	auto& camera_component = world.addComponent<ecs::CameraComponent>(camera);
}

void SceneHierarchy::createSkybox()
{
	Logger::Logger::get().trace("SceneHierarchy::createSkybox()");

	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "Skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	world.addComponent<ecs::UnpickableComponent>(skybox_entity);
	auto& skybox_renderable = world.addComponent<ecs::RenderableComponent>(skybox_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_cube_mesh();
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
	world.addComponent<ecs::SkyboxComponent>(skybox_entity).texture = Texture::generate_cube_map(faces);
	sub_mesh.material = std::make_shared<Asset::Material>();
	skybox_renderable.sub_meshes.push_back(sub_mesh);
}

void SceneHierarchy::createPlaneGround()
{
	Logger::Logger::get().trace("SceneHierarchy::createPlaneGround()");

	auto ground_entity = world.create_entity();
	auto ground_node = new ecs::Object(m_root_object, ground_entity);
	world.addComponent<ecs::NameComponent>(ground_entity).name = "Gound";
	world.addComponent<ecs::UnpickableComponent>(ground_entity);
	auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = Vec3(1.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_ground_mesh(Vec2(60.0f));
	std::shared_ptr<Asset::Material> ground_material = std::make_shared<Asset::Material>();
	ground_material->albedo = Vec3(1.0f, 1.0f, 1.0f);
	ground_material->metallic = 0.0f;
	ground_material->roughness = 1.0f;
	ground_material->ao = 0.01;
	sub_mesh.material = ground_material;
	ground_renderable.sub_meshes.push_back(sub_mesh);
}

void SceneHierarchy::createGridGround()
{
	Logger::Logger::get().trace("SceneHierarchy::createGridGround()");

	auto ground_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(ground_entity).name = "Grid";
	auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.translation = Vec3(0.0f, -0.05f, 0.0f);
	world.addComponent<ecs::UnpickableComponent>(ground_entity);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.sub_mesh_data = Asset::MeshData::create_ground_mesh(Vec2(1000.0f));
	sub_mesh.material = std::make_shared<Asset::Material>();;
	ground_renderable.sub_meshes.push_back(sub_mesh);
}

void SceneHierarchy::createDirectionalLight()
{
	Logger::Logger::get().trace("SceneHierarchy::createDirectionalLight()");

	auto dir_light_entity = world.create_entity();
	auto directional_light_node = new ecs::Object(m_root_object, dir_light_entity);
	world.addComponent<ecs::NameComponent>(dir_light_entity).name = "Directional Light";
	auto& dir_light_properties = world.addComponent<ecs::DirectionalLightComponent>(dir_light_entity);
	dir_light_properties.luminousColor = Color4(2.0f);
	auto& dir_light_transform = world.addComponent<ecs::TransformComponent>(dir_light_entity);
	dir_light_transform.translation = { -15.0f, 30.0f, -15.0f };
	dir_light_properties.direction = -dir_light_transform.translation;
}

void SceneHierarchy::init() {
	Logger::Logger::get().trace("SceneHierarchy::init()");

	auto root_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_entity).name = "Root";
	m_root_object = new ecs::Object(nullptr, root_entity);

	initMainCamera();

	createSkybox();

	createDirectionalLight();

	size_t point_lights_count = 2;
	auto root_point_lights_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_point_lights_entity).name = "Point Lights";
	m_root_point_light_object = new ecs::Object(m_root_object, root_point_lights_entity);
	for (int i = 0; i < point_lights_count; i++) {
		addPointLight();
	}

	size_t cubes_count = 36;
	auto root_cube_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_cube_entity).name = "Cubes";
	m_root_cube_object = new ecs::Object(m_root_object, root_cube_entity);
	for (int i = 0; i < cubes_count; i++) {
		addCube();
	}

	size_t spheres_count = 64;
	auto root_sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_sphere_entity).name = "Spheres";
	m_root_sphere_object = new ecs::Object(m_root_object, root_sphere_entity);
	for (int i = 0; i < spheres_count; i++) {
		addSphere();
	}

	createGridGround();

	createPlaneGround();

	//SceneHierarchy::loadModel(resource_dir + "/model/nanosuit/nanosuit.obj");

	ecs::Object* bunny_obj = SceneHierarchy::loadModel(resource_dir + "/model/bunny.obj");
	auto bunny_transform = world.getComponent<ecs::TransformComponent>(bunny_obj->entity());
	bunny_transform->scale = Vec3(40.0f);
	bunny_transform->translation = Vec3(-10.0f, 0.0f, 0.0f);

	//SceneHierarchy::loadModel(resource_dir + "/model/dragon.obj");

	Logger::Logger::get().trace("finished SceneHierarchy::init()");
}
