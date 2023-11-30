#include "LightingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

void LightingPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
	m_screen_quad = std::make_unique<ScreenQuad>();
	m_screen_quad->create();
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

	static Shader* lighting_shader = new Shader("resource/shader/lightingPass.vs", "resource/shader/lightingPass.fs");
	auto g_position_map = m_gbuffer_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
	lighting_shader->start_using();
	lighting_shader->setTexture("gPosition", 0, g_position_map);
	lighting_shader->setTexture("gNormal", 1, g_position_map + 1);
	lighting_shader->setTexture("gDiffuse", 2, g_position_map + 2);
	lighting_shader->setTexture("gSpecular", 3, g_position_map + 3);

	auto& world = ecs::World::get();
	glm::vec3 camera_pos;
	glm::mat4 camera_view = glm::mat4(1.0f);
	glm::mat4 camera_projection;
	for (auto entity : world.entityView<ecs::CameraComponent>()) {
		ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
		camera_pos = camera.pos;
		camera_view = camera.view;
		camera_projection = camera.projection;
	}
	lighting_shader->setFloat3("view_pos", camera_pos);
	glm::mat4 light_ref_matrix;
	int k = 0;
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->getLightProjMatrix();
		auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
		glm::vec3 light_pos = transform.transform()[3];
		glm::vec4 light_color = world.getComponent<ecs::LightComponent>(entity)->color;
		std::string light_id = std::string("lights[") + std::to_string(k) + "]";
		lighting_shader->setFloat4(light_id + ".color", light_color);
		lighting_shader->setFloat3(light_id + ".position", light_pos);
		k++;

		// ģ��������Ⱦ��Ƕ��forѭ�������������½�
		//for (int i = 0; i < 45 /*primitives count*/; i++) {
		//	std::string light_id = std::string("lights[") + std::to_string(i) + "]";
		//}
		// ����vs�ӳ���Ⱦ��
		// drawcall���ã�  renderable.primitives.size()��  vs  gbufferPass��renderable.primitives.size()�� + LightingPass 1�Ρ�
		// forѭ������shader��Դ���ԣ�  renderable.primitives.size() * Lights.size()��  vs  Lights.size()�Ρ�
		// shader���㣺  ÿ��drawcall��ÿ��VAO����ʱ��shader�Թ�դ�����Ƭ�β��м��㣬ÿ��shader�ڲ�ѭ��Lights.size()��
		//  vs  LightingPass��һ��drawcall�У�shader��������Ļ�������ز��м��㣬ÿ��shader�ڲ�ѭ��Lights.size()�Ρ�
	}
	if (m_shadow_map != 0) {
		lighting_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
		lighting_shader->setTexture("shadow_map", 4, m_shadow_map);
	}
	else {
		// set default map
		//shader->setTexture("shadow_map", 4, 0);
	}
	Renderer::drawTriangle(*lighting_shader, m_screen_quad->getVAO(), 6);
	lighting_shader->stop_using();

	// lights
	glEnable(GL_DEPTH_TEST);
	m_gbuffer_framebuffer->blitDepthMapTo(m_framebuffer.get());
	for (auto entity : world.entityView<ecs::LightComponent, ecs::RenderableComponent, ecs::TransformComponent>()) {
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