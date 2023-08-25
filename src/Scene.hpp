#ifndef Scene_hpp
#define Scene_hpp

#include "Object.hpp"
#include "View.hpp"
#include <unordered_map>

class Scene {
public:
	Scene() = default;
	~Scene() = default;
	Object* object(const std::string& name) {
		auto it = m_objects.find(name);
		if (it == m_objects.end())
			return nullptr;
		else
			return m_objects[name];
	}
	void insert_object(const std::string& name, Object* object) { m_objects.insert({ name, object }); }
	void destory_all_objects();
	void set_camera(Camera* camera) { m_camera = camera; }
	Camera* camera() { return m_camera; }
private:
	std::unordered_map<std::string, Object*> m_objects;
	Camera* m_camera{ nullptr };
};

#endif