#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "GamePlay/Framework/GObject.hpp"

class SceneHierarchy {
public:
	SceneHierarchy();
	~SceneHierarchy();

	GObject* rootObject() const { return m_root_object; }
	void addObject(GObject* obj, GObject* parent = nullptr);
	void removeObject(const ecs::Entity& entity);
	void removeObject(GObject* obj) { removeObject(obj->entity()); }
	GObject* object(const ecs::Entity& entity) { return m_root_object->find(entity); }

	void addPointLight();
	void addCube();
	void addSphere();
	void removeSphere(size_t index = -1);
	size_t pointLightCount() const { return m_point_light_count; }
	size_t sphereCount() const { return m_test_sphere_count; }

	GObject* loadModel(const std::string& filepath);
	void loadScene();

public:
	static size_t maxPointLightCount() { return 8; }

protected:
	void init();
	void initMainCamera(); // TODO 场景不应该包含摄像机
	void createSkybox();
	void createPlaneGround();
	void createGridGround();
	void createDirectionalLight();

	void updateSpheresPosition();

	void reset();

private:
	GObject* m_root_object{ nullptr };
	GObject* m_root_point_light_object{ nullptr };
	GObject* m_root_cube_object{ nullptr };
	GObject* m_root_sphere_object{ nullptr };
	size_t m_point_light_count = 0;
	size_t m_test_cube_count = 0;
	size_t m_test_sphere_count = 0;

	ecs::World& world;
};

#endif