#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "Logical/Framework/ECS/Object.hpp"

class SceneHierarchy {
public:
	SceneHierarchy();
	~SceneHierarchy();

	ecs::Object* rootObject() const { return m_root_object; }
	void addObject(ecs::Object* obj, ecs::Object* parent = nullptr);
	void removeObject(const ecs::Entity& entity);
	void removeObject(ecs::Object* obj) { removeObject(obj->entity()); }
	ecs::Object* object(const ecs::Entity& entity) { return m_root_object->find(entity); }

	void addPointLight();
	void addCube();
	void addSphere();
	void removeSphere(size_t index = -1);
	size_t pointLightCount() const { return m_point_light_count; }
	size_t sphereCount() const { return m_test_sphere_count; }

	ecs::Object* loadModel(const std::string& filepath);
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
	ecs::Object* m_root_object{ nullptr };
	ecs::Object* m_root_point_light_object{ nullptr };
	ecs::Object* m_root_cube_object{ nullptr };
	ecs::Object* m_root_sphere_object{ nullptr };
	size_t m_point_light_count = 0;
	size_t m_test_cube_count = 0;
	size_t m_test_sphere_count = 0;

	ecs::World& world;
};

#endif