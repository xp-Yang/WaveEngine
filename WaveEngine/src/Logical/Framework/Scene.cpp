#include "Scene.hpp"

#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"

#include "ResourceManager/AssetManager.hpp"

static const std::string resource_dir = RESOURCE_DIR;

void Scene::load()
{
}

void Scene::save()
{
}

#if ENABLE_ECS
GObject* Scene::loadModel(const std::string& filepath)
{
	Asset::ObjImporter obj_importer;
	obj_importer.load(filepath);
	std::vector<int> obj_sub_meshes_idx = obj_importer.getSubMeshesIds();
	if (obj_sub_meshes_idx.empty()) {
		//Logger::Logger::get().error("Model datas is empty. File loading fails. Please check if the filepath is all English.");
		return nullptr;
	}
	auto& world = ecs::World::get();
	auto entity = world.create_entity();
	std::string name = filepath.substr(filepath.find_last_of("/\\") + 1, filepath.find_last_of('.') - filepath.find_last_of("/\\") - 1);
	world.addComponent<ecs::NameComponent>(entity).name = name;
	world.addComponent<TransformComponent>(entity);
	//world.addComponent<ExplosionComponent>(entity);
	auto& renderable = world.addComponent<ecs::RenderableComponent>(entity);
	for (int idx : obj_sub_meshes_idx) {
		renderable.sub_meshes.push_back(Asset::SubMesh{ idx, Asset::MeshFileRef{ Asset::MeshFileType::OBJ, filepath}, {}, Mat4(1.0f) });
	}

	auto res = new GObject(m_root_object, entity);
	return res;
}
#endif

void Scene::init()
{
#if ENABLE_ECS
	auto& world = ecs::World::get();
	auto root_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_entity).name = "Root";
	m_root_object = new GObject(nullptr, root_entity);

	//initMainCamera
	auto camera = world.create_entity();
	world.addComponent<ecs::NameComponent>(camera).name = "Main Camera";
	auto& camera_component = world.addComponent<CameraComponent>(camera);

	//createSkybox
	auto skybox_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(skybox_entity).name = "Skybox";
	world.addComponent<ecs::TransformComponent>(skybox_entity);
	auto& skybox_com = world.addComponent<ecs::SkyboxComponent>(skybox_entity);
	skybox_com.cube_texture = Asset::CubeTexture(
		resource_dir + "/images/skybox/right.jpg",
		resource_dir + "/images/skybox/left.jpg",
		resource_dir + "/images/skybox/top.jpg",
		resource_dir + "/images/skybox/bottom.jpg",
		resource_dir + "/images/skybox/front.jpg",
		resource_dir + "/images/skybox/back.jpg");
	Asset::SubMesh sub_mesh;
	sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomCube, "" };
	skybox_com.mesh = sub_mesh;

	//createDirectionalLight
	auto dir_light_entity = world.create_entity();
	auto directional_light_node = new GObject(m_root_object, dir_light_entity);
	world.addComponent<ecs::NameComponent>(dir_light_entity).name = "Directional Light";
	auto& dir_light_properties = world.addComponent<ecs::DirectionalLightComponent>(dir_light_entity);
	dir_light_properties.luminousColor = Color4(2.0f);
	dir_light_properties.direction = { 15.0f, -30.0f, 15.0f };

	size_t point_lights_count = 2;
	auto root_point_lights_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_point_lights_entity).name = "Point Lights";
	auto m_root_point_light_object = new GObject(m_root_object, root_point_lights_entity);
	for (int i = 0; i < point_lights_count; i++) {
		auto point_light_entity = world.create_entity();
		auto point_light_node = new GObject(m_root_point_light_object, point_light_entity);
		world.addComponent<ecs::NameComponent>(point_light_entity).name = std::string("Point Light ") + std::to_string(i);
		auto& point_light_transform = world.addComponent<ecs::TransformComponent>(point_light_entity);
		double r1 = random(-15.0f, 15.0f);
		double r2 = random(3.0f, 10.0f);
		double r3 = random(-15.0f, 15.0f);
		point_light_transform.translation = { r1, r2, r3 };
		point_light_transform.scale = Vec3(random(0.1f, 0.3f));
		auto& point_light_com = world.addComponent<ecs::PointLightComponent>(point_light_entity);
		point_light_com.radius = (point_light_transform.scale[0]) * 100.0f;
		point_light_com.luminousColor = i == 0 ? Color4(2.0f, 2.0f, 2.0f, 1.0f) : Color4{ randomUnit(), randomUnit(), randomUnit(), 1.0f };
		Asset::SubMesh sub_mesh;
		sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomSphere, "" };
		point_light_com.mesh = sub_mesh;
	}

	auto updateSpheresPosition = [&](int sphere_count)
	{
		int one_sequence_max_num = 16;
		int max_cols = sqrt(one_sequence_max_num);
		int max_rows = max_cols;
		int sequences = sphere_count / one_sequence_max_num + 1;
		int cols = std::ceil(sqrt((sphere_count - 1) % one_sequence_max_num + 1));
		int rows = cols;

		float space = 3.0f;
		int i = 0;
		for (auto sphere_obj : m_root_sphere_object->children()) {
			auto& sphere_translation = world.getComponent<TransformComponent>(sphere_obj->entity())->translation;
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
	};

	size_t spheres_count = 64;
	auto root_sphere_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_sphere_entity).name = "Spheres";
	m_root_sphere_object = new GObject(m_root_object, root_sphere_entity);
	for (int i = 0; i < spheres_count; i++) {
		auto sphere_entity = world.create_entity();
		auto sphere_node = new GObject(m_root_sphere_object, sphere_entity);
		world.addComponent<ecs::NameComponent>(sphere_entity).name = std::string("Sphere") + std::to_string(i);
		world.addComponent<TransformComponent>(sphere_entity);

		auto& sphere_renderable = world.addComponent<ecs::RenderableComponent>(sphere_entity);
		Asset::SubMesh sub_mesh;
		sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomSphere, "" };
		sub_mesh.material.albedo = Vec3(1.0f, 1.0f, 1.0f);
		sub_mesh.material.metallic = 1.0;
		sub_mesh.material.roughness = (1.0f / 64) * (i + 1);
		sub_mesh.material.ao = 0.01;
		sphere_renderable.sub_meshes.push_back(sub_mesh);
		//world.addComponent<ExplosionComponent>(sphere_entity);

		i++;

		updateSpheresPosition(i);

		//Meta::Serialization::Serializer::saveToJsonFile(asset_dir + "/sphere.json", world.getComponent<TransformComponent>(sphere_entity));
		////TransformComponent trans;
		////Meta::Serialization::Serializer::loadFromJsonFile(asset_dir + "/sphere.json", &trans);
		//Meta::Serialization::Serializer::write(world.getComponent<TransformComponent>(sphere_entity));
	}

	// createGridGround
	auto ground_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(ground_entity).name = "Grid";
	auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.translation = Vec3(0.0f, -0.05f, 0.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomGrid, "" };
	ground_renderable.sub_meshes.push_back(sub_mesh);

	//createPlaneGround
	auto ground_entity = world.create_entity();
	auto ground_node = new GObject(m_root_object, ground_entity);
	world.addComponent<ecs::NameComponent>(ground_entity).name = "Gound";
	auto& ground_transform = world.addComponent<ecs::TransformComponent>(ground_entity);
	ground_transform.scale = Vec3(1.0f);
	auto& ground_renderable = world.addComponent<ecs::RenderableComponent>(ground_entity);
	Asset::SubMesh sub_mesh;
	sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomGround, "" };
	std::shared_ptr<Asset::Material> ground_material = std::make_shared<Asset::Material>();
	sub_mesh.material.albedo = Vec3(1.0f, 1.0f, 1.0f);
	sub_mesh.material.metallic = 0.0f;
	sub_mesh.material.roughness = 1.0f;
	sub_mesh.material.ao = 0.01;
	ground_renderable.sub_meshes.push_back(sub_mesh);

	loadModel(resource_dir + "/model/nanosuit/nanosuit.obj");

	GObject* bunny_obj = loadModel(resource_dir + "/model/bunny.obj");
	auto bunny_transform = world.getComponent<ecs::TransformComponent>(bunny_obj->entity());
	bunny_transform->scale = Vec3(40.0f);
	bunny_transform->translation = Vec3(-10.0f, 0.0f, 0.0f);

	//loadModel(resource_dir + "/model/dragon.obj");

#endif // ENABLE_ECS

	m_root_object = new GObject(nullptr, "Root");

	m_skybox = std::make_shared<Skybox>();

	m_light_manager = std::make_shared<LightManager>();
	m_light_manager->init();

	size_t cubes_count = 36;
	for (int i = 0; i < cubes_count; i++) {
		GObject* cube_obj = new GObject(m_root_object, "Cube");
		MeshComponent& mesh = cube_obj->addComponent<MeshComponent>();
		Asset::SubMesh cube_sub_mesh;
		cube_sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomCube, "" };
		cube_sub_mesh.material.albedo = Vec3(1.0f);
		cube_sub_mesh.material.metallic = 1.0;
		cube_sub_mesh.material.roughness = 0.5;
		cube_sub_mesh.material.ao = 0.01;
		mesh.sub_meshes.push_back(cube_sub_mesh);

		TransformComponent& transform = cube_obj->addComponent<TransformComponent>();
		transform.translation = { 1.5f * (i % 6), 0.5f + 1.5f * (i / 6), -10.0f };

		m_objects.push_back(std::shared_ptr<GObject>(cube_obj));
	}

	{
		GObject* plane_obj = new GObject(m_root_object, "Ground");
		MeshComponent& plane_mesh = plane_obj->addComponent<MeshComponent>();
		Asset::SubMesh plane_sub_mesh;
		plane_sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomGround, "" };
		plane_sub_mesh.material.albedo = Vec3(1.0f, 1.0f, 1.0f);
		plane_sub_mesh.material.metallic = 0.0f;
		plane_sub_mesh.material.roughness = 1.0f;
		plane_sub_mesh.material.ao = 0.01;
		plane_mesh.sub_meshes.push_back(plane_sub_mesh);

		TransformComponent& plane_transform = plane_obj->addComponent<TransformComponent>();

		m_objects.push_back(std::shared_ptr<GObject>(plane_obj));
	}
}

void Scene::onUpdate(float delta_time)
{
	for (const auto& obj : m_objects) {
		obj->tick(delta_time);
	}
}
