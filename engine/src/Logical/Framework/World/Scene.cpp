#include "Logical/Framework/World/Scene.hpp"

#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"
#include "Logical/Gcode/GcodeViewer.hpp"

#include "ResourceManager/ResourceImporter.hpp"
#include "ResourceManager/Gcode/GcodeImporter.hpp"

#include "Base/Logger/Logger.hpp"

static const std::string resource_dir = RESOURCE_DIR;

Scene::Scene()
{
	m_light_manager = std::make_shared<LightManager>();
	m_camera = std::make_shared<CameraComponent>(nullptr);
	m_gcode_viewer = std::make_shared<GcodeViewer>();
}

void Scene::load()
{
}

void Scene::save()
{
}

GObject* Scene::loadModel(const std::string& filepath)
{
	ResourceImporter model_importer;
	model_importer.load(filepath);
	std::vector<int> obj_sub_meshes_idx = model_importer.getSubMeshesIds();
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
		renderable.sub_meshes.push_back(Mesh{ idx, MeshFileRef{ MeshFileType::OBJ, filepath}, {}, Mat4(1.0f) });
	}
	auto res = GObject::create(nullptr, entity);
#else
	auto res = GObject::create(nullptr, name);
	res->addComponent<TransformComponent>();
	MeshComponent& mesh = res->addComponent<MeshComponent>();
	for (int idx : obj_sub_meshes_idx) {
		std::shared_ptr<Mesh> sub_mesh = model_importer.meshOfNode(idx);
		mesh.sub_meshes.push_back(sub_mesh);
	}
	m_objects.push_back(std::shared_ptr<GObject>(res));
#endif

	return res;
}

GCodeProcessorResult Scene::loadGcodeFile(const std::string& filepath)
{
	GCodeProcessor gcode_importer;
	gcode_importer.process_file(filepath);
	GCodeProcessorResult&& result = std::move(gcode_importer.extract_result());
	return result;
}

std::vector<GObjectID> Scene::getPickedObjectIDs() const
{
	std::vector<GObjectID> res(m_picked_objects.size());
	std::transform(m_picked_objects.begin(), m_picked_objects.end(), res.begin(), [](const std::shared_ptr<GObject>& obj) {
		return obj->ID();
		});
	return res;
}

void Scene::addObject(std::shared_ptr<GObject> obj)
{
	m_objects.push_back(std::shared_ptr<GObject>(obj));
}

std::shared_ptr<GcodeViewer> Scene::gcodeViewer() const
{
	return m_gcode_viewer;
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