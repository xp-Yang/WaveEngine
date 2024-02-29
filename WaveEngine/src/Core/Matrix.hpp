#ifndef Matrix_hpp
#define Matrix_hpp

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Core {

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

using Quat = glm::quat;

void LookAt();
void Perspective();
void Ortho();
void SVDDecomposition();
void Decomposition();
void Rotate();
void Translate();
void Scale();
void ShearX();
void ShearY();
void ShearZ();
void Inverse();
void Determinant();

}

using namespace Core;

#endif