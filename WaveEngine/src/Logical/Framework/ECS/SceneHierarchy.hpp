#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

#include "Core/Math.hpp"
#include "Logical/Framework/ECS/Object.hpp"

namespace ecs {

class SceneHierarchy {
public:
	SceneHierarchy();
	~SceneHierarchy();

	Object* rootObject() const { return m_root_object; }
	void addObject(Object* obj, Object* parent = nullptr);
	void removeObject(const Entity& entity);
	void removeObject(Object* obj) { removeObject(obj->entity()); }
	Object* object(const Entity& entity) { return m_root_object->find(entity); }

	void addPointLight();
	void addCube();
	void addSphere();
	void removeSphere(size_t index = -1);
	size_t pointLightCount() const { return m_point_light_count; }
	size_t sphereCount() const { return m_test_sphere_count; }

	Object* loadModel(const std::string& filepath);

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
	Object* m_root_object{ nullptr };
	Object* m_root_point_light_object{ nullptr };
	Object* m_root_cube_object{ nullptr };
	Object* m_root_sphere_object{ nullptr };
	size_t m_point_light_count = 0;
	size_t m_test_cube_count = 0;
	size_t m_test_sphere_count = 0;

	World& world;
};

}

#endif