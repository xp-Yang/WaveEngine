#ifndef Entity_hpp
#define Entity_hpp

namespace ecs{

// TODO ϣ����ĵط������ܴ�����ֻ��world���Դ���
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