#ifndef World_hpp
#define World_hpp

#include <bitset>
#include <vector>

namespace ecs{

const int MAX_ENTITIES = 32;
const int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

// TODO 实现Entity UUID
class Entity {
public:
    Entity() = delete;
    Entity(int id) { m_id = id; }
    int getId() const { return m_id; }

private:
    int m_id;
};

class ComponentPool
{
public:

    ComponentPool(size_t componentTypeSize)
    {
        m_componentTypeSize = componentTypeSize;
        m_data = new char[m_componentTypeSize * MAX_ENTITIES];
    }

    ~ComponentPool()
    {
        delete[] m_data;
    }

    inline void* get(size_t entity_id)
    {
        return m_data + entity_id * m_componentTypeSize;
    }

    char* m_data{ nullptr };
    size_t m_componentTypeSize{ 0 };
};

extern int g_componentCounter;
template <class T>
int getComponentId()
{
    static int componentId = g_componentCounter++;
    return componentId;
}

template<typename... ComponentTypes> class EnttView;
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
        int id = entity.getId();
        int component_id = getComponentId<T>();
        if (!m_entities[id].mask.test(component_id))
            return false;
        return true;
    }

    template<typename T>
    T* getComponent(ecs::Entity entity) 
    {
        int id = entity.getId();

        int component_id = getComponentId<T>();
        if (!m_entities[id].mask.test(component_id))
            return nullptr;

        T* component = static_cast<T*>(m_component_pools[component_id]->get(id));
        return component;
    }

    template<typename T>
    T& addComponent(ecs::Entity entity) 
    {
        int id = entity.getId();

        int component_id = getComponentId<T>();
        m_entities[id].mask.set(component_id);

        if (component_id >= m_component_pools.size()) {
            m_component_pools.push_back(new ComponentPool(sizeof(T)));
        }

        // Looks up the component in the pool, and initializes it with placement new
        T* component = new (m_component_pools[component_id]->get(id)) T();
        return *component;
    }

    // TODO
    template<typename T>
    void removeComponent(ecs::Entity entity) 
    {

    }

    template<typename... ComponentTypes>
    ecs::EnttView<ComponentTypes ...> entityView() {
        return EnttView<ComponentTypes ...>();
    }

    // TODO 需要封装
    //friend class EnttView;
//private:
    struct EntityDesc
    {
        Entity entity;
        ComponentMask mask;
    };

    std::vector<EntityDesc> m_entities;
    std::vector<ComponentPool*> m_component_pools;

private:
    World() = default;
    World(const World&) = default;
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
        Iter(World* _world, int entity_id, ComponentMask mask, bool all)
            : world(_world), entity_id(entity_id), mask(mask), all(all) {}

        Entity operator*() {
            return world->m_entities[entity_id].entity;
        }

        Iter& operator++() {
            entity_id++;
            for (; entity_id < world->m_entities.size(); entity_id++) {
                if (mask == (world->m_entities[entity_id].mask & mask))
                    break;
            }
            return *this;
        }

        Iter operator++(int) {
            Iter tmp = *this;
            ++* this;
            return tmp;
        }

        bool operator==(const Iter& another) const {
            return entity_id == another.entity_id;
        }

        bool operator!=(const Iter& another) const {
            return entity_id != another.entity_id;
        }

        int entity_id;
        ecs::World* world;
        ComponentMask mask;
        bool all{ false };
    };

    const Iter begin() const
    {
        int i = 0;
        for (i = 0; i < world->m_entities.size(); i++) {
            if ((world->m_entities[i].mask & m_mask) == m_mask)
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
        return Iter(world, (int)world->m_entities.size(), m_mask, all);
    }

    ecs::World* world{ nullptr };
    ComponentMask m_mask;
    bool all{ false };
};

}

#endif