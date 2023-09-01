#ifndef World_hpp
#define World_hpp

#include "Entity.hpp"
#include "Component.hpp"
#include <bitset>
#include <vector>

namespace ecs{

typedef std::bitset<MAX_COMPONENTS> ComponentMask;

class World {
public:
    // 单例
    static World& get() {
        static World* instance = new World();
        return *instance;
    }

    // 所有entity都从这里创建
    ecs::Entity create_entity();

    template<typename T>
    bool hasComponent(ecs::Entity entity)
    {
        int component_id = getComponentId<T>();
        if (!m_entities[entity].mask.test(component_id))
            return false;
        return true;
    }

    template<typename T>
    T* getComponent(ecs::Entity entity) 
    {
        int component_id = getComponentId<T>();
        if (!m_entities[entity].mask.test(component_id))
            return nullptr;

        T* pComponent = static_cast<T*>(m_component_pools[component_id]->get(entity));
        return pComponent;
    }

    template<typename T>
    T* addComponent(ecs::Entity entity) 
    {
        int component_id = getComponentId<T>();
        m_entities[entity].mask.set(component_id);

        if (component_id >= m_component_pools.size()) {
            m_component_pools.push_back(new ComponentPool(sizeof(T)));
        }

        // Looks up the component in the pool, and initializes it with placement new
        T* pComponent = new (m_component_pools[component_id]->get(entity)) T();
        return pComponent;
    }

    //template<typename T>
    //std::vector<ecs::Entity> getAllEntitiesOfComponent() 
    //{
    //    ;
    //}

    //friend class EnttView;
private:
    struct EntityDesc
    {
        Entity entity;
        ComponentMask mask;
    };

    std::vector<EntityDesc> m_entities;
    std::vector<ComponentPool*> m_component_pools;
};

template<typename... ComponentTypes>
class EnttView {
public:
    EnttView()
    {
        world = &World::get();
        if (sizeof...(ComponentTypes) == 0)
            all = true;
        else 
        {
            // Unpack the template parameters into an initializer list
            int componentIds[] = { 0, getComponentId<ComponentTypes>() ... };
            for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
                m_mask.set(componentIds[i]);
        }
    }

    class Iter {
    public:
        Iter(World* _world, int index, ComponentMask mask, bool all)
            : world(_world), index(index), mask(mask), all(all) {}

        Entity operator*() {
            return world->m_entities[index].entity;
        }

        Iter& operator++(){
            index++;
            for (; index < world->m_entities.size(); index++) {
                if (mask == (world->m_entities[index].mask & mask))
                    break;
            }
            return *this;
        }

        Iter operator++(int) {
            Iter tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const Iter& another) const {
            return index == another.index;
        }

        bool operator!=(const Iter& another) const {
            return index != another.index;
        }

        int index;
        ecs::World* world;
        ComponentMask mask;
        bool all{ false };
    };

    const Iter begin() const
    {
        int i = 0;
        for (i = 0; i < world->m_entities.size(); i++) {
            if(world->m_entities[i].mask & m_mask == m_mask)
                break;
        }
        return Iter(world, i, m_mask, all);
    }

    const Iter end() const
    {
        //int i = world->m_entities.size() - 1;
        //for (i = world->m_entities.size() - 1; i >= 0; i--) {
        //    if (world->m_entities[i].mask & m_mask == m_mask)
        //        break;
        //}
        //return Iter(world, i, m_mask, all);

        // 配合operator++()
        return Iter(world, world->m_entities.size(), m_mask, all);
    }

    ecs::World* world{ nullptr };
    ComponentMask m_mask;
    bool all{ false };
};

}

#endif