#ifndef Components_hpp
#define Components_hpp

#if ENABLE_ECS

#include "World.hpp"
#include "ResourceManager/AssetManager.hpp"
#include "Logical/Input/CameraManipulator.hpp"
#include "Engine.hpp"

namespace ecs {

struct NameComponent {
    std::string name;
};

struct TransformComponent {
	Vec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 rotation = { 0.0f, 0.0f, 0.0f }; //�Ƕ��� // TODO �������3������ת��˳��
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
	CameraManipulator::Mode mode{ CameraManipulator::Mode::Orbit };

	CameraManipulator::Projection projection_mode{ CameraManipulator::Projection::Perspective };

	CameraManipulator::ZoomMode zoom_mode{ CameraManipulator::ZoomMode::ZoomToCenter };

	// FPS style
	// ŷ���Ǳ�ʾ��
	struct FPSParams {
		float pitch = 0.0f; // ������������������ϵ x-z ƽ��ļн�
		float yaw = 0.0f; // ������������������ϵ x-z ƽ���ͶӰʸ�������������ϵ -z ��ļн�
		float roll = 0.0f;
	} fps_params;

	float originFov = deg2rad(45.0f); //��ֱfov

	float zoom = 1.0f;
	float fov = originFov / zoom;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	Vec3 direction = Normalize(Vec3(0.0f, -1.0f, -1.0f));
	Vec3 upDirection = Normalize(CameraManipulator::global_up - Dot(CameraManipulator::global_up, direction) * direction); // camera �� y ��
	Vec3 getRightDirection() const { // camera �� x ��
		return Cross(direction, upDirection);
	}
	Vec3 pos = Vec3(0.0f) - 40.0f * direction;
	Mat4 view = lookAt(pos, pos + direction, CameraManipulator::global_up);
	Mat4 projection = projection_mode == CameraManipulator::Projection::Perspective ?
		Math::Perspective(fov, ASPECT_RATIO, nearPlane, farPlane) :
		Math::Ortho(-15.0f * ASPECT_RATIO, 15.0f * ASPECT_RATIO, -15.0f, 15.0f, nearPlane, farPlane);
};

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
		auto& world = World::get();
		for (auto entity : world.entityView<PointLightComponent>()) {
			if (world.getComponent<PointLightComponent>(entity) == this) {
				return world.getComponent<TransformComponent>(entity)->translation;
			}
		}
		return{};
	}
	std::array<Mat4, 6> lightReferenceMatrix()
	{
		Mat4 light_projection = Perspective(deg2rad(90.0f), 1.0f, 0.1f, radius);
		Vec3 light_pos = position();

		// ����up�������£���ΪcubeMap���ڲ��������Ƿ�������
		// TODO ����Ӱ��ͼupʸ�����£���ô���²�Ҳ�ߵ���
		Mat4 light_view_right = LookAt(light_pos, light_pos + Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f)); //��
		Mat4 light_view_left = LookAt(light_pos, light_pos + Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f));//��
		Mat4 light_view_up = LookAt(light_pos, light_pos + Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)); //��
		Mat4 light_view_down = LookAt(light_pos, light_pos + Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));//��
		Mat4 light_view_front = LookAt(light_pos, light_pos + Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f)); //��
		Mat4 light_view_back = LookAt(light_pos, light_pos + Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f));//Զ
		
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
		Mat4 light_view = LookAt(Vec3(0.0f) - direction, Vec3(0.0f), CameraManipulator::global_up);
		return light_projection * light_view;
	}
};


// ����Ҫ���ݣ����������ѡ�е�entity
struct PickedComponent {};

struct ExplosionComponent {
	float explosionRatio = 0.0f;
};

#define AllComponents ecs::NameComponent, ecs::TransformComponent, ecs::CameraComponent, ecs::RenderableComponent, ecs::SkyboxComponent, ecs::PointLightComponent, ecs::DirectionalLightComponent, ecs::PickedComponent, ecs::ExplosionComponent

}

#endif

#endif