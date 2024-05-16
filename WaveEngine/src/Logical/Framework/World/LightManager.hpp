#ifndef LightManager_hpp
#define LightManager_hpp

#include "Core/Math.hpp"
#include <WaveEngine/Window.hpp>

struct Light {
	Color4 luminousColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct PointLightComponent : public Light {
	float radius;
	Vec3 position;

	std::array<Mat4, 6> lightReferenceMatrix() const
	{
		Mat4 light_projection = Perspective(deg2rad(90.0f), 1.0f, 0.1f, radius);
		Vec3 light_pos = position;

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

struct DirectionalLightComponent : public Light {
	Vec3 direction;

	Mat4 lightReferenceMatrix() const
	{
		Mat4 light_projection = Ortho(-30.0f * ASPECT_RATIO, 30.0f * ASPECT_RATIO, -30.0f, 30.0f, 0.1f, 100.0f);
		Mat4 light_view = LookAt(Vec3(0.0f) - direction, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
		return light_projection * light_view;
	}
};

class LightManager {
public:
	void init();
	const DirectionalLightComponent* mainDirectionalLight() const {
		// TODO
		return static_cast<DirectionalLightComponent*>(m_lights[0].get());
	}
	std::vector<const PointLightComponent*> pointLights() const {
		std::vector<const PointLightComponent*> res;
		for (const auto& light : m_lights) {
			if (light.get() != mainDirectionalLight())
				res.push_back(static_cast<const PointLightComponent*>(light.get()));
		}
		return res; 
	}

protected:
	void addPointLight();
	void addDirectionalLight();

private:
	std::vector<std::shared_ptr<Light>> m_lights;
};

#endif // !LightManager_hpp
