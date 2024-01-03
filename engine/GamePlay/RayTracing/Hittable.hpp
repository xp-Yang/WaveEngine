#ifndef Hittable_hpp
#define Hittable_hpp

#include "RayCast.hpp"

struct HitResult {
    RayCast ray;
    float t;
    Point3 result;
    Vec3 normal;
};

class Hittable {
public:
	virtual float hit(const RayCast& ray) = 0;

protected:
	virtual ~Hittable() = default;

};

class Sphere : public Hittable {
public:
    // equation: ||(origin + t * (dir) - center)|| = radius
	float hit(const RayCast& ray) override {
        Vec3 oc = ray.origin() - m_center;
        auto a = glm::dot(ray.direction(), ray.direction());
        auto b = 2.0 * dot(oc, ray.direction());
        auto c = dot(oc, oc) - m_radius * m_radius;
        auto discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return -1.0;
        }
        else {
            return (-b - sqrt(discriminant)) / (2.0 * a);
        }
	}

private:
    Point3 m_center;
    float m_radius;
};

#endif // !#Hittable_hpp
