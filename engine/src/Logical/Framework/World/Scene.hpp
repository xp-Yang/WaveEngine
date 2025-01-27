#ifndef Scene_hpp
#define Scene_hpp

#include "Logical/Framework/Object/GObject.hpp"
#include "Logical/Framework/Component/Component.hpp"
#include "Logical/Framework/World/LightManager.hpp"

#include "Logical/Gcode/GcodeViewer.hpp"

#include "ResourceManager/Gcode/GcodeImporter.hpp"

class GCodeProcessorResult;
class GCodeViewer;
class Scene {
public:
	Scene();

	void load();
	void save();

	GObject* loadModel(const std::string& filepath);
	GCodeProcessorResult loadGcodeFile(const std::string& filepath);
	const std::vector<std::shared_ptr<GObject>>& getPickedObjects() const { return m_picked_objects; }
	std::vector<GObjectID> getPickedObjectIDs() const;
	std::shared_ptr<Light> getPickedLight() const { return m_picked_light; }
	const std::vector<std::shared_ptr<GObject>>& getObjects() const { return m_objects; }
	std::shared_ptr<LightManager> getLightManager() const { return m_light_manager; }
	CameraComponent& getMainCamera() const { return *m_camera; }
	std::shared_ptr<GcodeViewer> gcodeViewer() const { return m_gcode_viewer; }

public slots:
	void onPickedChanged(std::vector<GObjectID> added, std::vector<GObjectID> removed);
	void onPickedChanged(LightID light_id);

private:
	std::vector<std::shared_ptr<GObject>> m_objects;
	std::shared_ptr<LightManager> m_light_manager;
	std::vector<std::shared_ptr<GObject>> m_picked_objects;
	std::shared_ptr<Light> m_picked_light;
	std::shared_ptr<CameraComponent> m_camera;
	std::shared_ptr<GcodeViewer> m_gcode_viewer;
};

#endif // !Scene_hpp
