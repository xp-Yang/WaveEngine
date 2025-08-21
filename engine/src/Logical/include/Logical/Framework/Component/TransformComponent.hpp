#ifndef TransformComponent_hpp
#define TransformComponent_hpp

#include "Logical/Framework/Component/Component.hpp"

struct TransformComponent : public Component {
	TransformComponent(GObject* parent) : Component(parent) {}

	Vec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 rotation = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };

	Mat4 transform() const
	{
		glm::qua<float> rotation_qua = glm::qua<float>(Vec3(Math::deg2rad(rotation.x), Math::deg2rad(rotation.y), Math::deg2rad(rotation.z)));
		Mat4 rotation = glm::mat4_cast(rotation_qua);
		Mat4 result = Math::Translate(translation)
			* rotation
			* Math::Scale(scale);
		return result;
	}
};

#endif // !TransformComponent_hpp
