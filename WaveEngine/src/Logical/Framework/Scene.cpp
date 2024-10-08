#include "Scene.hpp"

#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"

#include "ResourceManager/AssetManager.hpp"

#include "Core/Logger/Logger.hpp"

static const std::string resource_dir = RESOURCE_DIR;

void Scene::load()
{
}

void Scene::save()
{
}

GObject* Scene::loadModel(const std::string& filepath)
{
	Asset::ObjImporter obj_importer;
	obj_importer.load(filepath);
	std::vector<int> obj_sub_meshes_idx = obj_importer.getSubMeshesIds();
	if (obj_sub_meshes_idx.empty()) {
		//Logger::error("Model datas is empty. File loading fails. Please check if the filepath is all English.");
		return nullptr;
	}
	std::string name = filepath.substr(filepath.find_last_of("/\\") + 1, filepath.find_last_of('.') - filepath.find_last_of("/\\") - 1);

#if ENABLE_ECS
	auto& world = ecs::World::get();
	auto entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(entity).name = name;
	world.addComponent<TransformComponent>(entity);
	world.addComponent<ExplosionComponent>(entity);
	auto& renderable = world.addComponent<ecs::RenderableComponent>(entity);
	for (int idx : obj_sub_meshes_idx) {
		renderable.sub_meshes.push_back(Asset::SubMesh{ idx, Asset::MeshFileRef{ Asset::MeshFileType::OBJ, filepath}, {}, Mat4(1.0f) });
	}
	auto res = GObject::create(nullptr, entity);
#else
	auto res = GObject::create(nullptr, name);
	res->addComponent<TransformComponent>();
	MeshComponent& mesh = res->addComponent<MeshComponent>();
	for (int idx : obj_sub_meshes_idx) {
		mesh.sub_meshes.push_back(Asset::SubMesh{ idx, Asset::MeshFileRef{ Asset::MeshFileType::OBJ, filepath}, obj_importer.materialOfNode(idx), Mat4(1.0f) });
	}
	m_objects.push_back(std::shared_ptr<GObject>(res));
#endif

	return res;
}

std::vector<GObjectID> Scene::getPickedObjectIDs() const
{
	std::vector<GObjectID> res(m_picked_objects.size());
	std::transform(m_picked_objects.begin(), m_picked_objects.end(), res.begin(), [](const std::shared_ptr<GObject>& obj) {
		return obj->ID();
		});
	return res;
}

void Scene::init()
{
#if ENABLE_ECS
	auto& world = ecs::World::get();
	auto root_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_entity).name = "Root";

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
	auto directional_light_node = GObject::create(nullptr, dir_light_entity);
	world.addComponent<ecs::NameComponent>(dir_light_entity).name = "Directional Light";
	auto& dir_light_properties = world.addComponent<ecs::DirectionalLightComponent>(dir_light_entity);
	dir_light_properties.luminousColor = Color4(2.0f);
	dir_light_properties.direction = { 15.0f, -30.0f, 15.0f };

	size_t point_lights_count = 2;
	auto root_point_lights_entity = world.create_entity();
	world.addComponent<ecs::NameComponent>(root_point_lights_entity).name = "Point Lights";
	auto m_root_point_light_object = GObject::create(nullptr, root_point_lights_entity);
	for (int i = 0; i < point_lights_count; i++) {
		auto point_light_entity = world.create_entity();
		auto point_light_node = GObject::create(m_root_point_light_object, point_light_entity);
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
	m_root_sphere_object = GObject::create(nullptr, root_sphere_entity);
	for (int i = 0; i < spheres_count; i++) {
		auto sphere_entity = world.create_entity();
		auto sphere_node = GObject::create(m_root_sphere_object, sphere_entity);
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

	//createPlaneGround
	auto ground_entity = world.create_entity();
	auto ground_node = GObject::create(nullptr, ground_entity);
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

	m_skybox = std::make_shared<Skybox>();

	m_light_manager = std::make_shared<LightManager>();
	m_light_manager->init();

	size_t cubes_count = 9;
	size_t row_count = std::sqrt(cubes_count);
	size_t col_count = cubes_count / row_count;
	for (int i = 0; i < cubes_count; i++) {
		GObject* cube_obj = GObject::create(nullptr, "Cube");
		MeshComponent& mesh = cube_obj->addComponent<MeshComponent>();
		Asset::SubMesh cube_sub_mesh;
		cube_sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomCube, "" };
		cube_sub_mesh.material.albedo = Vec3(1.0f);
		cube_sub_mesh.material.metallic = 1.0;
		cube_sub_mesh.material.roughness = 0.5;
		cube_sub_mesh.material.ao = 0.01;
		mesh.sub_meshes.push_back(cube_sub_mesh);

		TransformComponent& transform = cube_obj->addComponent<TransformComponent>();
		transform.translation = { 1.5f * (i % col_count), 0.5f + 1.5f * (i / row_count), -10.0f };

		m_objects.push_back(std::shared_ptr<GObject>(cube_obj));
	}

	{
		GObject* plane_obj = GObject::create(nullptr, "Ground");
		MeshComponent& plane_mesh = plane_obj->addComponent<MeshComponent>();
		Asset::SubMesh plane_sub_mesh;
		plane_sub_mesh.mesh_file_ref = { Asset::MeshFileType::CustomGround, "" };
		plane_sub_mesh.material.albedo = Vec3(0.25f, 0.25f, 0.25f);
		plane_sub_mesh.material.metallic = 0.0f;
		plane_sub_mesh.material.roughness = 1.0f;
		plane_sub_mesh.material.ao = 0.01;
		plane_mesh.sub_meshes.push_back(plane_sub_mesh);

		TransformComponent& plane_transform = plane_obj->addComponent<TransformComponent>();
		plane_transform.scale = Vec3(50.f, 1.f, 50.f);

		m_objects.push_back(std::shared_ptr<GObject>(plane_obj));
	}

	//{
	//	GObject* nano_suit = loadModel(resource_dir + "/model/nanosuit/nanosuit.obj");
	//	nano_suit->getComponent<TransformComponent>()->scale = Vec3(0.3f);

	//	GObject* vampire = loadModel(resource_dir + "/model/vampire/dancing_vampire.dae");
	//	vampire->getComponent<TransformComponent>()->scale = Vec3(0.02f);
	//	vampire->getComponent<TransformComponent>()->translation = Vec3(5.0f, 0.0f, 0.0f);

	//	GObject* bunny_obj = loadModel(resource_dir + "/model/bunny.obj");
	//	auto bunny_transform = bunny_obj->getComponent<TransformComponent>();
	//	bunny_transform->scale = Vec3(30.0f);
	//	bunny_transform->translation = Vec3(-5.0f, 0.0f, 0.0f);
	//}
}

void Scene::onUpdate(float delta_time)
{
	for (const auto& obj : m_objects) {
		obj->tick(delta_time);
	}
}

void Scene::onPickedChanged(std::vector<GObjectID> added, std::vector<GObjectID> removed)
{
	m_picked_light.reset();
	m_picked_objects.erase(std::remove_if(m_picked_objects.begin(), m_picked_objects.end(), [removed](const std::shared_ptr<GObject>& obj) {
		return std::find(removed.begin(), removed.end(), obj->ID()) != removed.end();
		}), m_picked_objects.end());
	for (const auto& obj : m_objects) {
		if (std::find(added.begin(), added.end(), obj->ID()) != added.end()) {
			m_picked_objects.push_back(obj);
			Logger::debug("Scene::onPickedChanged(), added obj: {} {}", obj->ID().id, obj->name());
		}
	}
}

void Scene::onPickedChanged(LightID light_id)
{
	m_picked_light.reset();
	m_picked_objects.clear();
	const std::vector<std::shared_ptr<Light>>& lights = m_light_manager->lights();
	auto it = std::find_if(lights.begin(), lights.end(), [light_id](const std::shared_ptr<Light>& light) {
		return light->ID() == light_id;
		});
	if (it != lights.end()) {
		m_picked_light = *it;
		Logger::debug("Scene::onPickedChanged(), light: {} {}", m_picked_light->ID().id, m_picked_light->name());
	}
}