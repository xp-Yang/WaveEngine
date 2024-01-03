#ifndef RayCast_hpp
#define RayCast_hpp

#include "Core/Vector.hpp"

class RayCast {
public:
    RayCast() = default;
    RayCast(const Point3& origin, const Point3& direction)
        : m_origin(origin)
        , m_dir(direction)
    {}
    Point3 origin() const { return m_origin; }
    Vec3 direction() const { return m_dir; }
    Point3 at(float t) {
        return m_origin + m_dir * (float)t;
    }

private:
    Point3 m_origin;
    Vec3 m_dir;
};

#endif // !RayCast_hpp
