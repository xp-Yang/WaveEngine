#ifndef Engine_hpp
#define Engine_hpp

#include <string>
#include <memory>

static inline const std::string RESOURCE_DIRECTORY = RESOURCE_DIR;
static inline const std::string ASSET_DIRECTORY = ASSET_DIR;

static inline int    FRAMES_PER_SECOND = 60;
static inline double MILLISECONDS_PER_FRAME = 1000. / FRAMES_PER_SECOND;

class Scene;
class Engine {
public:
	~Engine();
	static Engine& get();
	void run();
	void init();
	void shutdown();

	std::shared_ptr<Scene> Scene() const;

private:
	Engine();
};

#endif
