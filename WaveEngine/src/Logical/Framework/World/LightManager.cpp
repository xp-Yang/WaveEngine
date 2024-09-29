#include "LightManager.hpp"

void LightManager::init()
{
	addDirectionalLight();

	for (int i = 0; i < 4; ++i)
		addPointLight();
}

void LightManager::addPointLight()
{
	std::shared_ptr<PointLight> point_light = std::make_shared<PointLight>();
	double r1 = random(-15.0f, 15.0f);
	double r2 = random(1.0f, 30.0f);
	double r3 = random(-15.0f, 15.0f);
	point_light->position = { r1, r2, r3 };
	point_light->radius = 30.f;// random(0.1f, 0.3f) * 100.0f;
	point_light->luminousColor = m_lights.size() == 0 ? Color4(3.0f, 3.0f, 3.0f, 1.0f) : Color4{randomUnit(), randomUnit(), randomUnit(), 1.0f};
	m_lights.push_back(point_light);
}

void LightManager::addDirectionalLight()
{
	std::shared_ptr<DirectionalLight> dir_light = std::make_shared<DirectionalLight>();
	dir_light->luminousColor = Color4(3.0f);
	dir_light->direction = { 15.0f, -30.0f, 15.0f };
	m_lights.push_back(dir_light);
}
