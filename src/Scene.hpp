#ifndef Scene_hpp
#define Scene_hpp

#include "Object.hpp"
#include "View.hpp"

class Scene {
public:
	Scene() = default;
	~Scene() = default;
	Object* object(int index) { return index < m_objects.size() ? m_objects[index] : nullptr; }
	std::vector<Object*> objects() { return m_objects; }
	void append_object(Object* object) { m_objects.push_back(object); }
	void destory_object(int index);
	void destory_object(Object* object);
	void destory_all_objects();
	void set_camera(Camera* camera) { m_camera = camera; }
	Camera* camera() { return m_camera; }
private:
	std::vector<Object*> m_objects;
	Camera* m_camera{ nullptr };
};

#endif