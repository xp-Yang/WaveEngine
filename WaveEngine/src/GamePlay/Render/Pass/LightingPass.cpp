#include "LightingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

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
	Vec3 camera_pos;
	Mat4 camera_view = Mat4(1.0f);
	Mat4 camera_projection;
	ecs::CameraComponent& camera = *world.getMainCameraComponent();
	camera_pos = camera.pos;
	camera_view = camera.view;
	camera_projection = camera.projection;

	// skybox
	for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
		auto skybox_texture_id = world.getComponent<ecs::SkyboxComponent>(entity)->texture;
		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			auto& material = renderable.primitives[i].material;
			Shader* shader = material.shader;
			material.update_shader_binding();
			shader->start_using();
			glDepthMask(GL_FALSE);
			shader->setMatrix("model", 1, model_matrix.transform());
			shader->setMatrix("view", 1, Mat4(Mat3(camera_view)));
			shader->setMatrix("projection", 1, camera_projection);
			shader->setCubeTexture("skybox", 6, skybox_texture_id);
			Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
			shader->stop_using();
		}
	}
	glDepthMask(GL_TRUE);

	// lighting
	Shader* lighting_shader = Shader::getShader(ShaderType::LightingShader);
	auto g_position_map = m_gbuffer_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
	lighting_shader->start_using();
	lighting_shader->setTexture("gPosition", 0, g_position_map);
	lighting_shader->setTexture("gNormal", 1, g_position_map + 1);
	lighting_shader->setTexture("gDiffuse", 2, g_position_map + 2);
	lighting_shader->setTexture("gSpecular", 3, g_position_map + 3);
	lighting_shader->setFloat3("view_pos", camera_pos);

	Mat4 light_ref_matrix;
	int k = 0;
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->getLightProjMatrix();
		auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
		Vec3 light_pos = transform.transform()[3];
		Vec4 light_color = world.getComponent<ecs::LightComponent>(entity)->color;
		std::string light_id = std::string("lights[") + std::to_string(k) + "]";
		lighting_shader->setFloat4(light_id + ".color", light_color);
		lighting_shader->setFloat3(light_id + ".position", light_pos);
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
	if (m_shadow_map != 0) {
		lighting_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
		lighting_shader->setTexture("shadow_map", 4, m_shadow_map);
	}
	else {
		// set default map
		//shader->setTexture("shadow_map", 4, 0);
	}
	Renderer::drawIndex(*lighting_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
	lighting_shader->stop_using();


	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	m_gbuffer_framebuffer->blitDepthMapTo(m_framebuffer.get());


	// lights
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			auto& material = renderable.primitives[i].material;
			Shader* shader = material.shader;
			material.update_shader_binding();
			shader->start_using();
			shader->setMatrix("model", 1, model_matrix.transform());
			shader->setMatrix("view", 1, camera_view);
			shader->setMatrix("projection", 1, camera_projection);
			Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
			shader->stop_using();
		}
	}

	// base grid ground
	for (auto entity : world.entityView<ecs::BaseGridGroundComponent>()) {
		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

		for (int i = 0; i < renderable.primitives.size(); i++) {
			auto& mesh = renderable.primitives[i].mesh;
			auto& material = renderable.primitives[i].material;
			Shader* shader = material.shader;
			material.update_shader_binding();
			shader->start_using();
			shader->setMatrix("model", 1, model_matrix.transform());
			shader->setMatrix("view", 1, camera_view);
			shader->setMatrix("projection", 1, camera_projection);
			Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
			shader->stop_using();
		}
	}
}

FrameBuffer* LightingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}
