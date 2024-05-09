#ifndef Scene_hpp
#define Scene_hpp

#include "Logical/Framework/Object/GObject.hpp"
#include "Logical/Framework/Component/Component.hpp"
#include "Logical/Framework/World/LightManager.hpp"
#include "Logical/Framework/World/Skybox.hpp"

class Scene {
public:
	void load();
	void save();

	void init();
	void createObject();

	void tick(float delta_time);

protected:
	std::vector<std::shared_ptr<GObject>> m_objects;
	std::shared_ptr<LightManager> m_light_manager;
	std::shared_ptr<Skybox> m_skybox;
};

#endif // !Scene_hpp
