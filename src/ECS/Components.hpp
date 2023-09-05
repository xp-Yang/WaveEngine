#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include "../Mesh.hpp"
#include "../Material.hpp"
#include "World.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define WINDOW_WIDTH 1600.0f
#define WINDOW_HEIGHT 900.0f

namespace ecs {

struct NameComponent {
    std::string name;
};

struct TransformComponent {
	// 如果想实现先平移后旋转，那么一直更新 translation = rotate_matrix * translation 就好了
	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 transform() const
	{
		glm::mat4 rotation = glm::mat4(1.0f);//glm::toMat4(glm::quat(rotation));
		glm::mat4 result = glm::translate(glm::mat4(1.0f), translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), scale);
		return result;
	}
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
	static glm::vec3 up;

	glm::vec3 pos = { 0.0f, 15.0f, 15.0f };
	glm::vec3 direction = glm::vec3(0.0f) - pos;
	glm::mat4 view = glm::lookAt(pos, pos+ direction, up);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
	float zoom = 1.0f;

	glm::vec3 get_right_direction() const { // camera 的 x 轴
		glm::vec3 cameraRight = glm::normalize(glm::cross(direction, up));
		for (int i = 0; i < 3; i++) {
			if (abs(cameraRight[i] - 1) < 0.001f) {
				cameraRight[i] = 1.0f;
				break;
			}
		}
		return cameraRight;
	}

	glm::vec3 get_up_direction() const { // camera 的 y 轴
		glm::vec3 cameraRight = get_right_direction();
		glm::vec3 cameraUp = glm::cross(cameraRight, direction);
		for (int i = 0; i < 3; i++) {
			if (abs(cameraUp[i] - 1) < 0.001f) {
				cameraUp[i] = 1.0f;
				break;
			}
		}
		return cameraUp;
	}
};

struct MeshComponent {
    std::vector<Mesh> meshes;
};

struct MaterialComponent {
    std::vector<Material> materials;
};

// 不需要数据，仅用作标记天空盒
struct SkyboxComponent {
	unsigned int texture;
};

struct LightComponent {
	glm::mat4 light_reference_matrix()
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
};

// 不需要数据，仅用作标记选中的entity
struct PickedComponent {};

struct ReceiveShadowComponent {};

struct ReceiveReflectionComponent {};

struct BoxCollider3DComponent {

};

struct BoxCollider2DComponent {

};

struct TextureComponent {
	unsigned int texture;
};

struct RendererComponent {

};


// Singleton Component
struct InputComponent {

};

}

#endif