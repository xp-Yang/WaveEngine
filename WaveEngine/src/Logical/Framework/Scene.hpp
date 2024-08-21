#ifndef Scene_hpp
#define Scene_hpp

#if ENABLE_ECS
#include "Logical/Framework/ECS/Object.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#else
#include "Logical/Framework/Object/GObject.hpp"
#include "Logical/Framework/Component/Component.hpp"
#include "Logical/Framework/World/LightManager.hpp"
#include "Logical/Framework/World/Skybox.hpp"
#endif

#include "Core/Signal/Signal.hpp"

class Scene {
public:
	void load();
	void save();

	void init();

	void onUpdate(float delta_time);
	GObject* rootObject() const { return m_root_object; }
	GObject* loadModel(const std::string& filepath);
#if ENABLE_ECS
	GObject* object(const Entity& entity) { return m_root_object->find(entity); }
#else
	const std::vector<std::shared_ptr<GObject>>& getPickedObjects() { return m_picked_objects; }
	std::vector<GObjectID> getPickedObjectIDs();
	const std::vector<std::shared_ptr<GObject>>& getObjects() { return m_objects; }
	std::shared_ptr<LightManager> getLightManager() { return m_light_manager; }
	std::shared_ptr<Skybox> getSkybox() { return m_skybox; }

	CameraComponent& getMainCamera() {
		static CameraComponent* camera = new CameraComponent(nullptr);
		return *camera;
}
#endif

public slots:
	void onPickedChanged(std::vector<GObjectID> added, std::vector<GObjectID> removed);

private:
	GObject* m_root_object{ nullptr };
#if ENABLE_ECS
	GObject* m_root_point_light_object{ nullptr };
	GObject* m_root_cube_object{ nullptr };
	GObject* m_root_sphere_object{ nullptr };
#else
	std::vector<std::shared_ptr<GObject>> m_objects;
	std::shared_ptr<LightManager> m_light_manager;
	std::shared_ptr<Skybox> m_skybox;
	std::vector<std::shared_ptr<GObject>> m_picked_objects;
#endif
};

#endif // !Scene_hpp
