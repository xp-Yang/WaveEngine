#ifndef World_hpp
#define World_hpp

#include <bitset>
#include <vector>

#include <assert.h>

namespace ecs{

const int MAX_ENTITIES = 512;
const int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

// TODO 实现Entity UUID
// 每个Entity保存一份bitset，大小是MAX_COMPONENTS。标记了该Entity有没有某种类型的Component数据
// World保存Entities数组
class Entity {
public:
    Entity() = delete;
    Entity(int id) { m_id = id; }
    Entity(const Entity& rhs) { m_id = rhs.getId(); m_mask = rhs.getMask(); }
    bool operator==(const Entity& rhs) const {
        return getId() == rhs.getId();
    }
    int getId() const { return m_id; }
    ComponentMask& getMask() { return m_mask; }
    const ComponentMask& getMask() const { return m_mask; }

private:
    int m_id;
    ComponentMask m_mask;
};

// 每个Pool保存相同类型的Component数组，数组大小是MAX_ENTITIES个。
// World保存Pools数组
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

// TODO 更改实现，有bug：get的不一定是已创建
// 为每个Pool分配id。由于每个Pool保存的都是同一类型的Component，这里用了hack: template<class T> 和 static 变量，使得只有不同的Pool才会初始化一次id
extern int g_componentCounter;
template <class T>
int getComponentPoolId()
{
    static int componentPoolId = g_componentCounter++;
    return componentPoolId;
}

class CameraComponent;
class DirectionalLightComponent;
class SkyboxComponent;
template<typename... ComponentTypes> class EnttView;
class World {
public:
    // 单例
    static World& get() {
        static World instance;
        return instance;
    }

    // 所有entity都从这里创建
    ecs::Entity create_entity()
    {
        static int entt_id = 0;
        m_entities.emplace_back(entt_id++);
        return m_entities.back();
    }

    void detroy_entity(const ecs::Entity& entity);

    template<typename T>
    bool hasComponent(ecs::Entity entity)
    {
        int entt_id = entity.getId();
        int pool_id = getComponentPoolId<T>();
        if (!m_entities[entt_id].getMask().test(pool_id))
            return false;
        return true;
    }

    template<typename T>
    T* getComponent(ecs::Entity entity) 
    {
        int entt_id = entity.getId();

        int pool_id = getComponentPoolId<T>();
        if (!m_entities[entt_id].getMask().test(pool_id))
            return nullptr;

        T* component = static_cast<T*>(m_component_pools[pool_id]->get(entt_id));
        return component;
    }

    template<typename T>
    T& addComponent(ecs::Entity entity) 
    {
        int entt_id = entity.getId();

        int pool_id = getComponentPoolId<T>();
        m_entities[entt_id].getMask().set(pool_id);

        if (pool_id >= m_component_pools.size()) {
            m_component_pools.push_back(new ComponentPool(sizeof(T)));
            assert(false);
        }

        // Looks up the component in the pool, and initializes it with placement new
        T* component = new (m_component_pools[pool_id]->get(entt_id)) T();
        return *component;
    }

    //template<typename T>
    //void removeComponent(ecs::Entity entity) 
    //{
    //    int id = entity.getId();

    //    int pool_id = getComponentPoolId<T>();
    //    m_entities[id].getMask().reset(pool_id);

    //    // 池中都没这个component
    //    if (pool_id >= m_component_pools.size()) {
    //        return;
    //    }
    //    else {
    //        // TODO 1.先要判断是否所有entity都不再使用这个component了
    //        // 2.要正确释放里面内容
    //        //m_component_pools.erase(m_component_pools.begin() + component_id);
    //    }
    //}

    template<typename... ComponentTypes>
    void removeComponent(ecs::Entity entity)
    {
        int id = entity.getId();

        // Unpack the template parameters into an initializer list
        int poolIds[] = { 0, getComponentPoolId<ComponentTypes>() ... };
        for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++) {
            m_entities[id].getMask().reset(poolIds[i]);

            // 池中都没这个component
            if (poolIds[i] >= m_component_pools.size()) {
                assert(false);
            }
            else {
                // free
            }
        }

    }

    template<typename... ComponentTypes>
    ecs::EnttView<ComponentTypes ...> entityView() {
        return EnttView<ComponentTypes ...>();
    }

    CameraComponent* getMainCameraComponent();

    DirectionalLightComponent* getMainDirectionalLightComponent();

    SkyboxComponent* getSkyboxComponent();

    std::vector<Entity> getPickedEntities();

    // TODO only for EnttView class, don't provide for public
    const std::vector<Entity>& getAllEntities() const {
        return m_entities;
    }

private:
    std::vector<Entity> m_entities;
    std::vector<ComponentPool*> m_component_pools;

private:
    World();
    World(const World&) = delete;
    ~World();

    template<typename T>
    void init_()
    {
        int pool_id = getComponentPoolId<T>();
        assert(pool_id == m_component_pools.size());
        m_component_pools.push_back(new ComponentPool(sizeof(T)));
    }

    template<typename... ComponentTypes>
    void init()
    {
        int poolIds[] = { 0, (init_<ComponentTypes>(), 0) ... };
    }
};

template<typename... ComponentTypes>
class EnttView {
public:
    EnttView()
        : world(&World::get())
    {
        if (sizeof...(ComponentTypes) == 0)
            view_all = true;
        else
        {
            // Unpack the template parameters into an initializer list
            int poolIds[] = { 0, getComponentPoolId<ComponentTypes>() ... };
            for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
                view_mask.set(poolIds[i]);
            view_all = false;
        }
    }

    class Iter {
    public:
        Iter(const EnttView* entt_view, int entity_idx)
            : entt_view_ref(entt_view), entity_idx(entity_idx) {}

        Entity operator*() {
            return entt_view_ref->world->getAllEntities()[entity_idx];
        }

        Iter& operator++() {
            entity_idx++;
            for (; entity_idx < entt_view_ref->world->getAllEntities().size(); entity_idx++) {
                if (entt_view_ref->view_all || (entt_view_ref->view_mask == (entt_view_ref->world->getAllEntities()[entity_idx].getMask() & entt_view_ref->view_mask)))
                    break;
            }
            return *this;
        }

        Iter operator++(int) {
            Iter tmp = *this;
            ++ *this;
            return tmp;
        }

        bool operator==(const Iter& another) const {
            return entity_idx == another.entity_idx;
        }

        bool operator!=(const Iter& another) const {
            return entity_idx != another.entity_idx;
        }

        int entity_idx;
        const EnttView* entt_view_ref{ nullptr };
    };

    const Iter begin() const
    {
        int i = 0;
        const auto& all_entities = world->getAllEntities();
        for (i = 0; i < all_entities.size(); i++) {
            if (view_all || ((all_entities[i].getMask() & view_mask) == view_mask))
                break;
        }
        // 如果没找到
        if (i == all_entities.size()) {
            return end();
        }
        return Iter(this, i);
    }

    const Iter end() const
    {
        // 配合operator++()
        return Iter(this, (int)world->getAllEntities().size());
    }

private:
    World* world{ nullptr };
    // 模板参数要查看的Component类型
    ComponentMask view_mask;
    bool view_all{ false };

    friend class Iter;
};

}

#endif