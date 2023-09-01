#ifndef Component_hpp
#define Component_hpp

namespace ecs {

const int MAX_ENTITIES = 32;
const int MAX_COMPONENTS = 32;

class ComponentPool
{
    ComponentPool(size_t componentTypeSize)
    {
        m_componentTypeSize = componentTypeSize;
        m_data = new char[m_componentTypeSize * MAX_ENTITIES];
    }
    
    ~ComponentPool()
    {
        delete[] m_data;
    }
    
    inline void* get(size_t entity)
    {
        return m_data + entity * m_componentTypeSize;
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

struct NameComponent {

};

struct PositionComponent {

};

struct TransformComponent {

};

struct MeshComponent {

};

struct MaterialComponent {

};

struct RendererComponent {

};

struct TextureComponent {

};


}

#endif