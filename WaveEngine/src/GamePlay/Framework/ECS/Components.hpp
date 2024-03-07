#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"
#include "World.hpp"
#include "ResourceManager/Mesh.hpp"
#include "ResourceManager/Material.hpp"
#include "Window_impl.hpp"

namespace ecs {


struct NameComponent {
    std::string name;
};

struct TransformComponent {
	Vec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 rotation = { 0.0f, 0.0f, 0.0f }; //角度制 // TODO 搞清楚绕3个轴旋转的顺序
	Vec3 scale = { 1.0f, 1.0f, 1.0f };

	Mat4 transform() const
	{
		glm::qua<float> rotation_qua = glm::qua<float>(Vec3(deg2rad(rotation.x), deg2rad(rotation.y), deg2rad(rotation.z)));
		Mat4 rotation = glm::mat4_cast(rotation_qua);
		Mat4 result = Translate(translation)
			* rotation
			* Scale(scale);
		return result;
	}
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

	enum ZoomMode {
		ZoomToCenter,
		ZoomToMouse,
	} zoom_mode{ ZoomToCenter };

	// FPS style
	// 欧拉角表示：
	struct FPSParams {
		float pitch = 0.0f; // 方向向量与世界坐标系 x-z 平面的夹角
		float yaw = 0.0f; // 方向向量在世界坐标系 x-z 平面的投影矢量相对世界坐标系 -z 轴的夹角
		float roll = 0.0f;
	} fps_params;

	static const float CameraMovementSpeed;
	static const float Sensitivity;
	static const float ZoomUnit;
	static Vec3 global_up; //vec3(0.0f, 1.0f, 0.0f) (y为上) or vec3(0.0f, 0.0f, 1.0f) (z为上)

	float originFov = deg2rad(45.0f); //竖直fov
	float zoom = 1.0f;
	float fov = originFov / zoom;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f , -1.0f));
	Vec3 pos = Vec3(0.0f) - 20.0f * direction;
	Vec3 camera_up = Normalize(global_up - Dot(global_up, direction) * direction);
	Mat4 view = lookAt(pos, pos + direction, global_up);
	Mat4 projection = projection_mode == Perspective ? Core::Perspective(fov, ASPECT_RATIO, 0.1f, 100.0f)
		: Core::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, 0.1f, 100.0f);

	Vec3 getRightDirection() const { // camera 的 x 轴
		return Cross(direction, camera_up);
		Vec3 cameraRight = Normalize(Cross(direction, global_up));
		return cameraRight;
	}

	Vec3 getUpDirection() const { // camera 的 y 轴
		return camera_up;
	}
};
inline const float CameraComponent::CameraMovementSpeed = 20.0f;
inline const float CameraComponent::Sensitivity = 0.01f;
inline const float CameraComponent::ZoomUnit = 0.1f;
inline Vec3 CameraComponent::global_up = Vec3(0.0f, 1.0f, 0.0f);

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

struct PointLightComponent {
	Color4 luminousColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float radius;
	Mat4 lightReferenceMatrix()
	{
		//lightProjection = Perspective(deg2rad(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		return {};
	}
};

struct DirectionalLightComponent {
	Vec3 direction;
	Color4 luminousColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	Mat4 lightReferenceMatrix()
	{
		auto& world = ecs::World::get();
		Mat4 light_projection = Ortho(-15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 15.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -15.0f, 15.0f, 0.1f, 100.0f);
		Mat4 light_view = LookAt(Vec3(0.0f) - direction, Vec3(0.0f), ecs::CameraComponent::global_up);
		Mat4 ret = light_projection * light_view;
		return ret;
	}
};


// TODO 决定一个data是放在component的属性里还是单独作为一个component的关键判据是什么
// 为什么Transform总是与Renderable分离

// 不需要数据，仅用作标记选中的entity
struct PickedComponent {};

struct BaseGridGroundComponent {};

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