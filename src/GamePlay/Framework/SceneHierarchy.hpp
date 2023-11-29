#ifndef SceneHierarchy_hpp
#define SceneHierarchy_hpp

class SceneHierarchy {
public:
	SceneHierarchy();
	~SceneHierarchy();
	//Object* object(const std::string& name) {
	//	auto it = m_objects.find(name);
	//	if (it == m_objects.end())
	//		return nullptr;
	//	else
	//		return m_objects[name];
	//}
	//Object* object(const std::string& name) const {
	//	auto it = m_objects.find(name);
	//	if (it == m_objects.end())
	//		return nullptr;
	//	else
	//		return m_objects.at(name);
	//}
	//const std::unordered_map<std::string, Object*>& get_objects() const { return m_objects; }
	//bool insert_object(const std::string& name, Object* object) { return m_objects.insert({ name, object }).second; }
	//void remove_object(const std::string& name) { m_objects.erase(name); };
	//void set_light(MyLight* light) { m_light = light; }
	//const MyLight& get_light() const { return *m_light; }
	//void set_skybox(Skybox* skybox) { m_skybox = skybox; }
	//const Skybox& get_skybox() const { return *m_skybox; }

protected:
	void init();

private:

};

#endif