#ifndef RayCast_hpp
#define RayCast_hpp

#include "glm/glm.hpp"

class RayCast {
public:
    RayCast() = default;
    RayCast(const glm::vec3& origin, const glm::vec3& direction) 
        :m_origin(origin)
        ,m_dir(direction)
    {}
    glm::vec3 at(float t) {
        return m_origin + m_dir * (float)t;
    }

private:
    glm::vec3 m_origin;
    glm::vec3 m_dir;
};

#endif // !RayCast_hpp
