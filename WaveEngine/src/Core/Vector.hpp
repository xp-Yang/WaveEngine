#ifndef Vector_hpp
#define Vector_hpp

#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Core {

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Point3 = glm::vec3;
using Point4 = glm::vec4;
using Points3 = std::vector<Point3>;
using Points4 = std::vector<Point4>;

using Color4 = glm::vec4;

template<typename Vec>
float Dot(const Vec& vec1, const Vec& vec2) {
    return glm::dot(vec1, vec2);
}

template<typename Vec>
Vec Cross(const Vec& vec1, const Vec& vec2) {
    return glm::cross(vec1, vec2);
}

template<typename Vec>
Vec Normalize(const Vec& vec) {
    return glm::normalize(vec);
}

template<typename Vec>
Vec Reflect(Vec v, Vec n) {
    return v + 2 * Dot(-v, n) * n;
}

}

using namespace Core;

#endif // !Vector_hpp

