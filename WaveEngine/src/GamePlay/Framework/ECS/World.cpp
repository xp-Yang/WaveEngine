#include "World.hpp"
#include "Components.hpp"

namespace ecs {

int g_componentCounter = 0;

CameraComponent* World::getMainCameraComponent()
{
    for (auto entity : entityView<CameraComponent>()) {
        CameraComponent* camera = getComponent<CameraComponent>(entity);
        return camera;
    }
    return nullptr;
}

DirectionalLightComponent* World::getMainDirectionalLightComponent()
{
    for (auto entity : entityView<DirectionalLightComponent>()) {
        DirectionalLightComponent* directionalLight = getComponent<DirectionalLightComponent>(entity);
        return directionalLight;
    }
    return nullptr;
}

SkyboxComponent* World::getSkyboxComponent()
{
    for (auto entity : entityView<SkyboxComponent>()) {
        SkyboxComponent* skybox = getComponent<SkyboxComponent>(entity);
        return skybox;
    }
    return nullptr;
}

std::vector<Entity> World::getPickedEntities()
{
    std::vector<Entity> result;
    for (auto entity : entityView<PickedComponent>()) {
        result.push_back(entity);
    }
    return result;
}

World::World()
{
    init<AllComponents>();
}

World::~World()
{
    for (int i = 0; i < m_component_pools.size(); i++)
        delete m_component_pools[i];
}

}