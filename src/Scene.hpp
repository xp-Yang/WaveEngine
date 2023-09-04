#ifndef Scene_hpp
#define Scene_hpp

#include "ECS/World.hpp"
#include "Object.hpp"
#include "MyLight.hpp"
#include "Skybox.hpp"
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
	Object* object(const std::string& name) const {
		auto it = m_objects.find(name);
		if (it == m_objects.end())
			return nullptr;
		else
			return m_objects.at(name);
	}
	const std::unordered_map<std::string, Object*>& get_objects() const { return m_objects; }
	bool insert_object(const std::string& name, Object* object) { return m_objects.insert({ name, object }).second; }
	void remove_object(const std::string& name) { m_objects.erase(name); };
	void set_light(MyLight* light) { m_light = light; }
	void set_skybox(Skybox* skybox) { m_skybox = skybox; }
	const MyLight& get_light() const { return *m_light; }
	const Skybox& get_skybox() const { return *m_skybox; }

	void init();

private:
	std::unordered_map<std::string, Object*> m_objects;
	MyLight* m_light{ nullptr };
	Skybox* m_skybox{ nullptr };

};

#endif