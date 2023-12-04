#ifndef Math_hpp
#define Math_hpp

#include <cmath>
#include <random>
#include "Vector.hpp"

namespace MathUtils {
	inline constexpr double PI = 3.1415926535897932385;
	inline constexpr double epsilon = 1e-6;
	inline float deg2rad(float deg) { return deg * PI / 180.0f; }
	inline float rad2deg(float rad) { return rad * 180.0f / PI; }
    inline float randomUnit() {
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
        //// Returns a random real in [0,1).
        //return rand() / (RAND_MAX + 1.0);
    }
    inline float random(float min, float max) {
        // Returns a random real in [min,max).
        return min + (max - min) * randomUnit();
    }
    // generate a vector on a unit sphere
    inline Vec3 randomUnitVec() {
        while (true) {
            Vec3 unit_cube(randomUnit(), randomUnit(), randomUnit());
            if (glm::dot(unit_cube, unit_cube) < 1.0f) { //为了均匀分布，否则归一化后沿立方体对角线的抽样比较多
                return glm::normalize(unit_cube);
            }
        }
        return Vec3(1.0f, 0, 0);
    }
    inline Vec3 randomOnHemisphere(const Vec3& normal) {
        Vec3 unit = randomUnitVec();
        if (glm::dot(unit, normal) > 0.0)
            return unit;
        else
            return -unit;
    }
    inline Point3 getPointOnUnitSphere(const Point3& plane_point, const Vec3& normal) {
        float r_square = glm::dot(plane_point, plane_point);
        if (r_square > 1)
            return{};

        float h = 1 - r_square;

        return plane_point + h * normal;
    }
    inline Vec3 randomLambertianDistribution(const Vec3& normal) {
        // 1. 构造切平面
        Vec3 up = Vec3(0, 1, 0);
        Vec3 local_u;
        if (normal == up)
            local_u = Vec3(1, 0, 0);
        local_u = glm::cross(normal, up);
        Vec3 local_v = glm::cross(normal, local_u);

        // 2. 在切平面均匀取点
        Point3 random_plane_point;
        random_plane_point = randomUnit() * local_u + randomUnit() * local_v;

        // 3. 将点映射回球面
        return getPointOnUnitSphere(random_plane_point, normal);
    }

    inline double clamp(double x, double min, double max) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
    inline bool isApproxZero(float real) {
        return -epsilon < real && real < epsilon;
    }
    inline bool isApproxZero(Vec3 vec) {
        return isApproxZero(vec.x) && isApproxZero(vec.y) && isApproxZero(vec.z);
    }
    inline Vec3 reflect(Vec3 v, Vec3 n) {
        return v + 2 * glm::dot(-v, n) * n;
    }
}

struct Interval {
    double min, max;

    Interval() : min(+FLT_MAX), max(-FLT_MAX) {} // Default interval is empty
    Interval(double _min, double _max) : min(_min), max(_max) {}
    bool contains(double x) const {
        return min <= x && x <= max;
    }
    bool surrounds(double x) const {
        return min < x && x < max;
    }
};

#endif // !MathHpp
