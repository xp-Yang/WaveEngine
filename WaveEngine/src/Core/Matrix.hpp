#ifndef Matrix_hpp
#define Matrix_hpp

#include "Vector.hpp"

namespace Core {

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;


inline Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    return glm::lookAt(eye, center, up);
}

inline Mat4 Perspective(float fovy, float aspect_ratio, float zNear, float zFar) {
    return glm::perspective(fovy, aspect_ratio, zNear, zFar);
}

inline Mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
    return glm::ortho(left, right, bottom, top, zNear, zFar);
}


inline Mat4 Scale(const Vec3& scale) {
    return glm::scale(Mat4(1.0f), scale);
}

inline Mat4 Rotate(float angle, const Vec3& rotate_axis) {
    return glm::rotate(Mat4(1.0f), angle, rotate_axis);
}

inline Mat4 Translate(const Vec3& translation) {
    return glm::translate(Mat4(1.0f), translation);
}

void ShearX();
void ShearY();
void ShearZ();

template <typename Mat>
Mat Inverse(const Mat& mat) {
    return glm::inverse(mat);
}
void Determinant();

void SVDDecomposition();
void Decomposition();

}

using namespace Core;

#endif