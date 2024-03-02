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

std::vector<Entity> World::getPickedEntities()
{
    std::vector<Entity> result;
    for (auto entity : entityView<PickedComponent>()) {
        result.push_back(entity);
    }
    return result;
}

World::~World()
{
    for (int i = 0; i < m_component_pools.size(); i++)
        delete m_component_pools[i];
}

}