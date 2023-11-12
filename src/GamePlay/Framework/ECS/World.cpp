#include "World.hpp"

namespace ecs {

int g_componentCounter = 0;

World::~World()
{
    for (int i = 0; i < m_component_pools.size(); i++)
        delete m_component_pools[i];
}

}