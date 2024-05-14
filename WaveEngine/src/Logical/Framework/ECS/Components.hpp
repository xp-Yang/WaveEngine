#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include "Core/Math.hpp"
#include "World.hpp"
#include "ResourceManager/AssetManager.hpp"
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
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f , -1.0f));
	Vec3 pos = Vec3(0.0f) - 40.0f * direction;
	Vec3 camera_up = Normalize(global_up - Dot(global_up, direction) * direction);
	Mat4 view = lookAt(pos, pos + direction, global_up);
	Mat4 projection = projection_mode == Perspective ? Math::Perspective(fov, ASPECT_RATIO, nearPlane, farPlane)
		: Math::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, nearPlane, farPlane);

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

struct RenderableComponent {
	std::vector<Asset::SubMesh> sub_meshes;
};

struct SkyboxComponent {
	Asset::CubeTexture cube_texture;
	Asset::SubMesh mesh;
};

struct PointLightComponent {
	Color4 luminousColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float radius;
	Asset::SubMesh mesh;
	Vec3 position() {
		auto& world = ecs::World::get();
		for (auto entity : world.entityView<ecs::PointLightComponent>()) {
			if (world.getComponent<ecs::PointLightComponent>(entity) == this) {
				return world.getComponent<ecs::TransformComponent>(entity)->translation;
			}
		}
		return{};
	}
	std::array<Mat4, 6> lightReferenceMatrix()
	{
		Mat4 light_projection = Perspective(deg2rad(90.0f), 1.0f, 0.1f, radius);
		Vec3 light_pos = position();

		// 这里up向量朝下，因为cubeMap从内部采样，是反过来的
		// TODO 点阴影贴图up矢量朝下，那么上下不也颠倒了
		Mat4 light_view_right = LookAt(light_pos, light_pos + Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f)); //右
		Mat4 light_view_left = LookAt(light_pos, light_pos + Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));//左
		Mat4 light_view_up = LookAt(light_pos, light_pos + Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)); //上
		Mat4 light_view_down = LookAt(light_pos, light_pos + Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));//下
		Mat4 light_view_front = LookAt(light_pos, light_pos + Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f)); //近
		Mat4 light_view_back = LookAt(light_pos, light_pos + Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f));//远
		
		std::array<Mat4, 6> result;
		result[0] = (light_projection * light_view_right);
		result[1] = (light_projection * light_view_left);
		result[2] = (light_projection * light_view_up);
		result[3] = (light_projection * light_view_down);
		result[4] = (light_projection * light_view_front);
		result[5] = (light_projection * light_view_back);
		return result;
	}
};

struct DirectionalLightComponent {
	Color4 luminousColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vec3 direction;

	Mat4 lightReferenceMatrix()
	{
		Mat4 light_projection = Ortho(-30.0f * ASPECT_RATIO, 30.0f * ASPECT_RATIO, -30.0f, 30.0f, 0.1f, 100.0f);
		Mat4 light_view = LookAt(Vec3(0.0f) - direction, Vec3(0.0f), ecs::CameraComponent::global_up);
		return light_projection * light_view;
	}
};


// 不需要数据，仅用作标记选中的entity
struct PickedComponent {};

struct ExplosionComponent {
	float explosionRatio = 0.0f;
};


#define AllComponents ecs::NameComponent, ecs::TransformComponent, ecs::CameraComponent, ecs::RenderableComponent, ecs::SkyboxComponent, ecs::PointLightComponent, ecs::DirectionalLightComponent, ecs::PickedComponent, ecs::ExplosionComponent

}

#endif