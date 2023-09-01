#include "World.hpp"

namespace ecs {

ecs::Entity World::create_entity() 
{
    static int entt_id = 0;
    ecs::Entity entt(++entt_id);
    m_entities.push_back({ entt, ComponentMask()});
    return m_entities.back().entity;
}

}