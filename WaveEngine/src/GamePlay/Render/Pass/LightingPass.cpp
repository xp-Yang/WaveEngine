#include "LightingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

#include "Application_impl.hpp"
#include "GamePlay/Framework/SceneHierarchy.hpp"

void LightingPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
	m_screen_quad = Mesh::create_screen_mesh();
}

void LightingPass::prepare(FrameBuffer* framebuffer)
{
	m_gbuffer_framebuffer = framebuffer;
	m_shadow_map = 0;
}

void LightingPass::prepare(FrameBuffer* g_fb, FrameBuffer* shadow_fb)
{
	m_gbuffer_framebuffer = g_fb;
	m_shadow_map = shadow_fb->getFirstAttachmentOf(AttachmentType::DEPTH).getMap();
}

void LightingPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

	auto& world = ecs::World::get();
	ecs::CameraComponent& camera = *world.getMainCameraComponent();
	auto dir_light_component = world.getMainDirectionalLightComponent();
	Mat4 light_ref_matrix = dir_light_component->lightReferenceMatrix();

	// wireframe
	if (m_wireframe) {
		drawWireframeMode();
		if (m_normal_debug) {
			drawNormalMode();
		}
		return;
	}

	// checkerboard
	if (m_checkerboard) {
		drawCheckerboardMode();
		if (m_normal_debug) {
			drawNormalMode();
		}
		return;
	}

	// deferred lighting
	Shader* lighting_shader = Shader::getShader(ShaderType::DeferredLightingShader);
	auto g_position_map = m_gbuffer_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
	lighting_shader->start_using();
	lighting_shader->setTexture("gPosition", 0, g_position_map);
	lighting_shader->setTexture("gNormal", 1, g_position_map + 1);
	lighting_shader->setTexture("gDiffuse", 2, g_position_map + 2);
	lighting_shader->setTexture("gSpecular", 3, g_position_map + 3);
	// PBR
	lighting_shader->setTexture("gAlbedo", 4, g_position_map + 4);
	lighting_shader->setTexture("gMetallic", 5, g_position_map + 5);
	lighting_shader->setTexture("gRoughness", 6, g_position_map + 6);
	lighting_shader->setTexture("gAo", 7, g_position_map + 7);

	lighting_shader->setBool("enablePBR", m_pbr);

	lighting_shader->setFloat3("cameraPos", camera.pos);

	Vec3 light_direction = dir_light_component->direction;
	Vec4 light_color = dir_light_component->luminousColor;
	lighting_shader->setFloat3("directionalLight.direction", light_direction);
	lighting_shader->setFloat4("directionalLight.color", light_color);

	if (m_shadow_map != 0) {
		lighting_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
		lighting_shader->setTexture("shadow_map", 4, m_shadow_map);

		for (int i = 0; i < m_cube_maps.size(); i++) {
			std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
			// TODO 纹理单元的绑定搞清楚。
			lighting_shader->setCubeTexture(cube_map_id, 9 + i, m_cube_maps[i]);
		}
	}

	int k = 0;
	for (auto entity : world.entityView<ecs::PointLightComponent>()) {
		auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
		Vec3 light_pos = transform.transform()[3];
		Vec4 light_color = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
		float light_radius = world.getComponent<ecs::PointLightComponent>(entity)->radius;
		std::string light_id = std::string("pointLights[") + std::to_string(k) + "]";
		lighting_shader->setFloat3(light_id + ".position", light_pos);
		lighting_shader->setFloat4(light_id + ".color", light_color);
		lighting_shader->setFloat(light_id + ".radius", light_radius);
		k++;

		// 模拟正向渲染的嵌套for循环，性能明显下降
		//for (int i = 0; i < 45 /*primitives count*/; i++) {
		//	std::string light_id = std::string("lights[") + std::to_string(i) + "]";
		//}
		// 正向vs延迟渲染：
		// drawcall调用：  renderable.primitives.size()次  vs  gbufferPass：renderable.primitives.size()次 + LightingPass 1次。
		// for循环设置shader光源属性：  renderable.primitives.size() * Lights.size()次  vs  Lights.size()次。
		// shader计算：  每次drawcall，每个VAO绘制时，shader对光栅化后的片段并行计算，每个shader内部循环Lights.size()次
		//  vs  LightingPass的一次drawcall中，shader对整个屏幕所有像素并行计算，每个shader内部循环Lights.size()次。
	}
	lighting_shader->setInt("point_lights_size", k);

	Renderer::drawIndex(*lighting_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
	lighting_shader->stop_using();


	// lights
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	// TODO 拷贝深度图后才能画法线
	m_gbuffer_framebuffer->blitDepthMapTo(m_framebuffer.get());
	for (auto entity : world.entityView<ecs::PointLightComponent>()) {
		auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			auto& material = renderable.primitives[i].material;
			Shader* shader = material.shader;
			material.update_shader_binding();
			shader->start_using();
			shader->setFloat4("color", point_light.luminousColor);
			shader->setMatrix("model", 1, model_matrix.transform());
			shader->setMatrix("view", 1, camera.view);
			shader->setMatrix("projection", 1, camera.projection);
			Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
			shader->stop_using();
		}
	}

	// normal
	if (m_normal_debug) {
		drawNormalMode();
	}

	// skybox
	if (m_skybox) {
		for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
			auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
			auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
			if (world.getPickedEntities().empty())
				break;
			auto point_light_entity = world.getPickedEntities()[0];
			if (!world.hasComponent<ecs::PointLightComponent>(point_light_entity))
				break;
			int debug_index = Application::GetApp().getSceneHierarchy()->rootObject()->find(point_light_entity)->index();
			auto skybox_texture_id = m_cube_maps[debug_index];// world.getComponent<ecs::SkyboxComponent>(entity)->texture;
			for (int i = 0; i < renderable.primitives.size(); i++) {
				auto& mesh = renderable.primitives[i].mesh;
				auto& material = renderable.primitives[i].material;
				Shader* shader = material.shader;
				material.update_shader_binding();
				shader->start_using();
				shader->setMatrix("model", 1, model_matrix.transform());
				shader->setMatrix("view", 1, Mat4(Mat3(camera.view)));
				shader->setMatrix("projection", 1, camera.projection);
				shader->setCubeTexture("skybox", 6, skybox_texture_id);
				Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
				shader->stop_using();
			}
		}
	}
}

FrameBuffer* LightingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}

void LightingPass::enableSkybox(bool enable)
{
	m_skybox = enable;
}

void LightingPass::enableNormal(bool enable)
{
	m_normal_debug = enable;
}

void LightingPass::enableWireframe(bool enable)
{
	m_wireframe = enable;
}

void LightingPass::enableCheckerboard(bool enable)
{
	m_checkerboard = enable;
}

void LightingPass::enablePBR(bool enable)
{
	m_pbr = enable;
}

void LightingPass::drawNormalMode()
{
	auto& world = ecs::World::get();
	ecs::CameraComponent& camera = *world.getMainCameraComponent();

	Shader* normal_shader = Shader::getShader(ShaderType::NormalShader);
	normal_shader->start_using();
	normal_shader->setMatrix("view", 1, camera.view);
	normal_shader->setMatrix("projection", 1, camera.projection);
	normal_shader->setMatrix("projectionView", 1, camera.projection * camera.view);
	for (auto entity : world.entityView<ecs::RenderableComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		normal_shader->start_using();
		normal_shader->setMatrix("model", 1, model_matrix.transform());
		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			Renderer::drawIndex(*normal_shader, mesh.get_VAO(), mesh.get_indices_count());
		}
	}
}

void LightingPass::drawWireframeMode()
{
	auto& world = ecs::World::get();
	ecs::CameraComponent& camera = *world.getMainCameraComponent();

	Shader* wireframe_shader = Shader::getShader(ShaderType::WireframeShader);
	wireframe_shader->start_using();
	wireframe_shader->setMatrix("view", 1, camera.view);
	wireframe_shader->setMatrix("projection", 1, camera.projection);
	for (auto entity : world.entityView<ecs::RenderableComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		wireframe_shader->start_using();
		wireframe_shader->setMatrix("model", 1, model_matrix.transform());
		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			Renderer::drawIndex(*wireframe_shader, mesh.get_VAO(), mesh.get_indices_count());
		}
	}
}

void LightingPass::drawCheckerboardMode()
{
	auto& world = ecs::World::get();
	ecs::CameraComponent& camera = *world.getMainCameraComponent();
	auto dir_light_component = world.getMainDirectionalLightComponent();

	Mat4 light_ref_matrix = dir_light_component->lightReferenceMatrix();
	Shader* grid_shader = Shader::getShader(ShaderType::CheckerboardShader);
	for (auto entity : world.entityView<ecs::RenderableComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			grid_shader->start_using();
			grid_shader->setMatrix("modelScale", 1, Scale(model_matrix.scale));
			grid_shader->setMatrix("model", 1, model_matrix.transform());
			grid_shader->setMatrix("view", 1, camera.view);
			grid_shader->setMatrix("projection", 1, camera.projection);
			if (m_shadow_map != 0) {
				grid_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
				grid_shader->setTexture("shadow_map", 4, m_shadow_map);
			}
			Renderer::drawIndex(*grid_shader, mesh.get_VAO(), mesh.get_indices_count());
			grid_shader->stop_using();
		}
	}
}
