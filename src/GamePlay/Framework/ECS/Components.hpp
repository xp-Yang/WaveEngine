#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "World.hpp"
#include "ResourceManager/Mesh.hpp"
#include "ResourceManager/Material.hpp"
#include "Window.hpp"

namespace ecs {

struct NameComponent {
    std::string name;
};

struct TransformComponent {
	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; //角度制
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 transform() const
	{
		glm::qua<float> rotation_qua = glm::qua<float>(glm::radians(rotation));
		glm::mat4 rotation = glm::mat4_cast(rotation_qua);
		glm::mat4 result = glm::translate(glm::mat4(1.0f), translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), scale);
		return result;
	}

	// TODO 写个decomposition 和 composition
};

struct CameraComponent {
	//FPS style
	//struct Direction {
	//	float pitch = 0.0f; // 方向向量与世界坐标系 x-z 平面的夹角
	//	float yaw = 0.0f; // 方向向量在世界坐标系 x-z 平面的投影矢量相对世界坐标系 -z 轴的夹角
	//	float roll = 0.0f;
	//};
	static const float CameraMovementSpeed;
	static const float Sensitivity;
	static const float ZoomUnit;
	static glm::vec3 up; //vec3(0.0f, 1.0f, 0.0f) (y为上) or vec3(0.0f, 0.0f, 1.0f) (z为上)

	float focal_length = {20.0f};
	glm::vec3 direction = glm::normalize(glm::vec3(0.0f, -1.0f , -1.0f));
	glm::vec3 pos = glm::vec3(0.0f) - focal_length * direction;
	glm::mat4 view = glm::lookAt(pos, pos + direction, up);
	//glm::mat4 projection = glm::ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(fov, ASPECT_RATIO, 0.1f, 100.0f);
	float originFov = glm::radians(45.0f);
	float zoom = 1.0f;
	float fov = glm::radians(45.0f); // fov = originFov / zoom

	glm::vec3 getRightDirection() const { // camera 的 x 轴
		glm::vec3 cameraRight = glm::normalize(glm::cross(direction, up));
		//for (int i = 0; i < 3; i++) {
		//	if (abs(cameraRight[i] - 1) < 0.001f) {
		//		cameraRight[i] = 1.0f;
		//		break;
		//	}
		//}
		return cameraRight;
	}

	glm::vec3 getUpDirection() const { // camera 的 y 轴
		glm::vec3 cameraRight = getRightDirection();
		glm::vec3 cameraUp = glm::cross(cameraRight, direction);
		//for (int i = 0; i < 3; i++) {
		//	if (abs(cameraUp[i] - 1) < 0.001f) {
		//		cameraUp[i] = 1.0f;
		//		break;
		//	}
		//}
		return cameraUp;
	}
};

enum class PrimitiveType {
	POINTS         = 0,
    LINES          = 1,
    LINE_STRIP     = 3,
    TRIANGLES      = 4,
    TRIANGLE_STRIP = 5 
};

struct Primitive {
	PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
	Mesh mesh;
	Material material;
};

struct RenderableComponent {
	std::vector<Primitive> primitives;
	void setPrimitives(const std::vector<Primitive>& _primitives) {
		primitives = _primitives;
	}
	//BoundingBox boundingBox;
	//bool culling;
	//int priority;
	//骨骼
};

struct SkyboxComponent {
	unsigned int texture;
};

struct LightComponent {
	glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	glm::mat4 lightReferenceMatrix()
	{
		auto& world = ecs::World::get();
		glm::mat4 light_projection = glm::ortho(-15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -15.0f, 15.0f, 0.1f, 100.0f);
		//lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::vec3 light_pos = glm::vec3(0.0f);
		for (auto entity : world.entityView<ecs::LightComponent>()) {
			light_pos = world.getComponent<ecs::TransformComponent>(entity)->translation;
		}
		glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), ecs::CameraComponent::up);
		glm::mat4 ret = light_projection * light_view;
		return ret;
	}
	glm::mat4 getLightProjMatrix() {
		glm::mat4 result = lightReferenceMatrix();
		return result;
	}
};

// TODO 决定一个data是放在component的属性里还是单独作为一个component的关键判据是什么
// 为什么Transform总是与Renderable分离

// 不需要数据，仅用作标记选中的entity
struct PickedComponent {};

//struct ReceiveShadowComponent {};
//
//struct ReceiveReflectionComponent {};

struct BoxCollider3DComponent {

};

struct BoxCollider2DComponent {

};

struct ExplosionComponent {
	float explosionRatio = 0.0f;
};


// Singleton Component
struct InputComponent {

};

}

#endif