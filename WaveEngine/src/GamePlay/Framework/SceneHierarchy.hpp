#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "GamePlay/Framework/GameObject.hpp"

class SceneHierarchy {
public:
	SceneHierarchy();
	~SceneHierarchy();

	GameObject* rootObject() const { return m_root_object; }
	void addObject(GameObject* obj, GameObject* parent = nullptr);
	void removeObject(const ecs::Entity& entity);
	void removeObject(GameObject* obj) { removeObject(obj->entity()); }
	GameObject* object(const ecs::Entity& entity) { return m_root_object->find(entity); }

	void addPointLight();
	void addCube();
	void addSphere();
	void removeSphere(size_t index = -1);
	size_t maxPointLightCount() const { return 8; }
	size_t pointLightCount() const { return m_point_light_count; }
	size_t sphereCount() const { return m_test_sphere_count; }

	GameObject* loadModel(const std::string& filepath);
	void loadScene();

protected:
	void init();
	void initMainCamera(); // TODO 场景不应该包含摄像机
	void createSkybox();
	void createGround();
	void createDirectionalLight();

	void updateSpheresPosition();

	void reset();

private:
	GameObject* m_root_object{ nullptr };
	GameObject* m_root_point_light_object{ nullptr };
	GameObject* m_root_cube_object{ nullptr };
	GameObject* m_root_sphere_object{ nullptr };
	size_t m_point_light_count = 0;
	size_t m_test_cube_count = 0;
	size_t m_test_sphere_count = 0;

	ecs::World& world;
};

#endif