#include "World.hpp"
#include "Components.hpp"

namespace ecs {

int g_componentCounter = 0;

CameraComponent* World::getMainCameraComponent()
{
    for (auto entity : entityView<CameraComponent>()) {
        CameraComponent* camera = getComponent<CameraComponent>(entity);
        if (camera)
            return camera;
    }
}

World::~World()
{
    for (int i = 0; i < m_component_pools.size(); i++)
        delete m_component_pools[i];
}

}