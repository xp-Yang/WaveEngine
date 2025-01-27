#ifndef IScene_hpp
#define IScene_hpp

class Object;
class IScene {
public:
    virtual void addObject();
    virtual void removeObject();
    virtual	GObject* loadModel(const std::string& filepath);
    virtual const std::vector<std::shared_ptr<GObject>>& getObjects() const;
	virtual std::shared_ptr<LightManager> getLightManager() const;
	virtual CameraComponent& getMainCamera() const;
	virtual std::shared_ptr<GcodeViewer> gcodeViewer() const;
};

#endif
