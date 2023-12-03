#ifndef Math_hpp
#define Math_hpp

#include <cmath>
#include <random>
#include "Vector.hpp"

namespace MathUtils {
	constexpr double PI = 3.1415926535897932385;
	constexpr double epsilon = 1e-6;
	float deg2rad(float deg) { return deg * PI / 180.0f; }
	float rad2deg(float rad) { return rad * 180.0f / PI; }
    float randomUnit() {
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
        //// Returns a random real in [0,1).
        //return rand() / (RAND_MAX + 1.0);
    }
    float random(float min, float max) {
        // Returns a random real in [min,max).
        return min + (max - min) * randomUnit();
    }
    // generate a vector on a unit sphere
    Vec3 randomUnitVec() {
        while (true) {
            Vec3 unit_cube(randomUnit(), randomUnit(), randomUnit());
            if (glm::dot(unit_cube, unit_cube) < 1.0f) { //为了均匀分布，否则归一化后沿立方体对角线的抽样比较多
                return glm::normalize(unit_cube);
            }
        }
        return Vec3(1.0f, 0, 0);
    }
    Vec3 randomOnHemisphere(const Vec3& normal) {
        Vec3 unit = randomUnitVec();
        if (glm::dot(unit, normal) > 0.0)
            return unit;
        else
            return -unit;
    }
    double clamp(double x, double min, double max) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
    bool isApproxZero(float real) {
        return -epsilon < real && real < epsilon;
    }
    bool isApproxZero(Vec3 vec) {
        return isApproxZero(vec.x) && isApproxZero(vec.y) && isApproxZero(vec.z);
    }
    Vec3 reflect(Vec3 v, Vec3 n) {
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
