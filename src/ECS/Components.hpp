#ifndef Components_hpp
#define Components_hpp

#include <string>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../Mesh.hpp"
#include "../Material.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define WINDOW_WIDTH 1600.0f
#define WINDOW_HEIGHT 900.0f

namespace ecs {

struct NameComponent {
    std::string name;
};

struct TransformComponent {
	// �����ʵ����ƽ�ƺ���ת����ôһֱ���� translation = rotate_matrix * translation �ͺ���
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
	//	float pitch = 0.0f; // ������������������ϵ x-z ƽ��ļн�
	//	float yaw = 0.0f; // ������������������ϵ x-z ƽ���ͶӰʸ�������������ϵ -z ��ļн�
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

	const glm::vec3& get_right_direction() const { // camera �� x ��
		glm::vec3 cameraRight = glm::normalize(glm::cross(direction, up));
		for (int i = 0; i < 3; i++) {
			if (abs(cameraRight[i] - 1) < 0.001f) {
				cameraRight[i] = 1.0f;
				break;
			}
		}
		return cameraRight;
	}

	const glm::vec3& get_up_direction() const { // camera �� y ��
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

// ����Ҫ���ݣ������������պ�
struct SkyboxComponent {
	unsigned int texture;
};

// ����Ҫ���ݣ���������Դ
struct LightComponent {};

// ����Ҫ���ݣ����������ѡ�е�entity
struct PickedComponent {};

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