#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include "Core/Vector.hpp"
#include "World.hpp"
#include "ResourceManager/Mesh.hpp"
#include "ResourceManager/Material.hpp"
#include "Window.hpp"

namespace ecs {

struct NameComponent {
    std::string name;
};

struct TransformComponent {
	Vec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 rotation = { 0.0f, 0.0f, 0.0f }; //�Ƕ���
	Vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 transform() const
	{
		glm::qua<float> rotation_qua = glm::qua<float>(glm::radians(rotation));
		glm::mat4 rotation = glm::mat4_cast(rotation_qua);
		glm::mat4 result = glm::translate(glm::mat4(1.0f), translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), scale);
		return result;
	}

	// TODO д��decomposition �� composition
};

struct CameraComponent {
	enum Mode {
		Orbit,
		FPS,
	} mode{ Orbit };

	enum Projection {
		Perspective,
		Ortho,
	} projection_mode{ Perspective };

	// FPS style
	// ŷ���Ǳ�ʾ��
	struct FPSParams {
		float pitch = 0.0f; // ������������������ϵ x-z ƽ��ļн�
		float yaw = 0.0f; // ������������������ϵ x-z ƽ���ͶӰʸ�������������ϵ -z ��ļн�
		float roll = 0.0f;
	} fps_params;

	static const float CameraMovementSpeed;
	static const float Sensitivity;
	static const float ZoomUnit;
	static Vec3 up; //vec3(0.0f, 1.0f, 0.0f) (yΪ��) or vec3(0.0f, 0.0f, 1.0f) (zΪ��)

	float originFov = glm::radians(45.0f); //ˮƽfov
	float zoom = 1.0f;
	float fov = originFov / zoom;
	float focal_length = {20.0f};
	Vec3 direction = glm::normalize(Vec3(0.0f, -1.0f , -1.0f));
	Vec3 pos = Vec3(0.0f) - focal_length * direction;
	Vec3 camera_up = glm::normalize(up - glm::dot(up, direction) * direction);
	glm::mat4 view = glm::lookAt(pos, pos + direction, up);
	glm::mat4 projection = projection_mode == Perspective ? glm::perspective(fov, ASPECT_RATIO, 0.1f, 100.0f)
		: glm::ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, 0.1f, 100.0f);

	Vec3 getRightDirection() const { // camera �� x ��
		return glm::cross(direction, camera_up);
		Vec3 cameraRight = glm::normalize(glm::cross(direction, up));
		return cameraRight;
	}

	Vec3 getUpDirection() const { // camera �� y ��
		return camera_up;
		Vec3 cameraRight = getRightDirection();
		Vec3 cameraUp = glm::cross(cameraRight, direction);
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
	//����
};

struct SkyboxComponent {
	unsigned int texture;
};

struct LightComponent {
	Color4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	glm::mat4 lightReferenceMatrix()
	{
		auto& world = ecs::World::get();
		glm::mat4 light_projection = glm::ortho(-15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -15.0f, 15.0f, 0.1f, 100.0f);
		//lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		Vec3 light_pos = Vec3(0.0f);
		for (auto entity : world.entityView<ecs::LightComponent>()) {
			light_pos = world.getComponent<ecs::TransformComponent>(entity)->translation;
		}
		glm::mat4 light_view = glm::lookAt(light_pos, Vec3(0.0f, 0.0f, 0.0f), ecs::CameraComponent::up);
		glm::mat4 ret = light_projection * light_view;
		return ret;
	}
	glm::mat4 getLightProjMatrix() {
		glm::mat4 result = lightReferenceMatrix();
		return result;
	}
};

// TODO ����һ��data�Ƿ���component�������ﻹ�ǵ�����Ϊһ��component�Ĺؼ��о���ʲô
// ΪʲôTransform������Renderable����

// ����Ҫ���ݣ����������ѡ�е�entity
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