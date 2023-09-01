#ifndef Entity_hpp
#define Entity_hpp

namespace ecs{

// TODO 希望别的地方都不能创建，只有world可以创建
class Entity {
public:
    Entity() = default;
    Entity(int id) { m_id = id; }
    int getId() const { return m_id; }

private:
    int m_id;
};

}

#endif