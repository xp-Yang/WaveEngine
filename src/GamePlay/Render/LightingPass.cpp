#include "LightingPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

void LightingPass::init()
{
    m_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
	m_screen_quad = new ScreenQuad();
	m_screen_quad->create();
}

void LightingPass::prepare(FrameBuffer* framebuffer)
{
	m_gbuffer_framebuffer = framebuffer;
}

void LightingPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

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
	int k = 0;
	for (auto entity : world.entityView<ecs::LightComponent>()) {
		auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
		glm::vec3 light_pos = transform.transform()[3];
		glm::vec4 light_color = world.getComponent<ecs::RenderableComponent>(entity)->primitives[0].material.color;
		std::string light_id = std::string("lights[") + std::to_string(k) + "]";
		lighting_shader->setFloat4(light_id + ".color", light_color);
		lighting_shader->setFloat3(light_id + ".position", light_pos);
		k++;
	}
	Renderer::drawTriangle(*lighting_shader, m_screen_quad->getVAO(), 6);
	lighting_shader->stop_using();

	// lights
	glEnable(GL_DEPTH_TEST);
	m_gbuffer_framebuffer->blitDepthMapTo(m_framebuffer);
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
	return m_framebuffer;
}
