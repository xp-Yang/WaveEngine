#include "Components.hpp"

namespace ecs {

const float CameraComponent::CameraMovementSpeed = 20.0f;
const float CameraComponent::Sensitivity = 0.01f;
const float CameraComponent::ZoomUnit = 0.1f;
Vec3 CameraComponent::global_up = Vec3(0.0f, 1.0f, 0.0f);

}