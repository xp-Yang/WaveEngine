#include "LightingPass.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

#include "Application_impl.hpp"
#include "Logical/Framework/SceneHierarchy.hpp"

void LightingPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });

	m_lights_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_lights_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
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
	m_lights_framebuffer->bind();
	m_lights_framebuffer->clear(Color3(0.0f));

	m_framebuffer->bind();
	m_framebuffer->clear();

	auto& world = ecs::World::get();
	ecs::CameraComponent& camera = *world.getMainCameraComponent();

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

	for (const auto& render_directional_light_data : m_render_source_data->render_directional_light_data_list) {
		lighting_shader->setFloat3("directionalLight.direction", render_directional_light_data.direction);
		lighting_shader->setFloat4("directionalLight.color", render_directional_light_data.color);
		if (m_shadow_map != 0) {
			lighting_shader->setMatrix("lightSpaceMatrix", 1, render_directional_light_data.lightReferenceMatrix);
			lighting_shader->setTexture("shadow_map", 8, m_shadow_map);
		}
	}

	//for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
		if (m_shadow_map != 0) {
			for (int i = 0; i < m_cube_maps.size(); i++) {
				std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
				lighting_shader->setCubeTexture(cube_map_id, 9 + i, m_cube_maps[i]);
			}
		}
	//}

	// 模拟正向渲染的嵌套for循环，性能明显下降
	// 正向vs延迟渲染：
	// drawcall调用：  renderable.sub_meshes.size()次  vs  gbufferPass：renderable.sub_meshes.size()次 + LightingPass 1次。
	// for循环设置shader光源属性：  renderable.sub_meshes.size() * Lights.size()次  vs  Lights.size()次。
	// shader计算：  每次drawcall，每个VAO绘制时，shader对光栅化后的片段并行计算，每个shader内部循环Lights.size()次
	//  vs  LightingPass的一次drawcall中，shader对整个屏幕所有像素并行计算，每个shader内部循环Lights.size()次。
	int point_light_idx = 0;
	for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
		std::string light_id = std::string("pointLights[") + std::to_string(point_light_idx) + "]";
		lighting_shader->setFloat3(light_id + ".position", render_point_light_data.position);
		lighting_shader->setFloat4(light_id + ".color", render_point_light_data.color);
		lighting_shader->setFloat(light_id + ".radius", render_point_light_data.radius);
		point_light_idx++;
	}
	lighting_shader->setInt("point_lights_size", m_render_source_data->render_point_light_data_list.size());
	Renderer::drawIndex(*lighting_shader, m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	lighting_shader->stop_using();


	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	// TODO 拷贝深度图后才能画法线
	m_gbuffer_framebuffer->blitDepthMapTo(m_framebuffer.get());
	// grid
	//for (auto entity : world.entityView<ecs::RenderableComponent>()) {
	//	if (world.getComponent<ecs::NameComponent>(entity)->name == "Grid") {
	//		Shader* wireframe_shader = Shader::getShader(ShaderType::WireframeShader);
	//		auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
	//		auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
	//		wireframe_shader->start_using();
	//		wireframe_shader->setMatrix("view", 1, camera.view);
	//		wireframe_shader->setMatrix("projection", 1, camera.projection);
	//		wireframe_shader->setMatrix("model", 1, model_matrix.transform());
	//		for (int i = 0; i < renderable.sub_meshes.size(); i++) {
	//			auto& mesh = renderable.sub_meshes[i].mesh;
	//			Renderer::drawIndex(*wireframe_shader, mesh.get_VAO(), mesh.get_indices_count());
	//		}
	//	}
	//}
	
	// lights
	for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
		for (const auto& render_point_light_sub_mesh_data : render_point_light_data.render_mesh_data.render_sub_mesh_data_list) {
			auto& material = render_point_light_sub_mesh_data.material();
			Shader* shader = material->shader;
			material->update_shader_binding();
			shader->start_using();
			shader->setFloat4("color", render_point_light_data.color);
			shader->setMatrix("model", 1, render_point_light_data.render_mesh_data.model_matrix);
			shader->setMatrix("view", 1, camera.view);
			shader->setMatrix("projection", 1, camera.projection);
			Renderer::drawIndex(*shader, render_point_light_sub_mesh_data.getVAO(), render_point_light_sub_mesh_data.indicesCount());
			shader->stop_using();
		}
	}

	// normal
	if (m_normal_debug) {
		drawNormalMode();
	}

	// skybox
	if (m_skybox) {
		for (const auto& render_skybox_sub_mesh_data : m_render_source_data->render_skybox_data.render_mesh_data.render_sub_mesh_data_list) {
			auto& material = render_skybox_sub_mesh_data.material();
			material->update_shader_binding();
			Shader* shader = material->shader;
			shader->start_using();
			shader->setMatrix("model", 1, m_render_source_data->render_skybox_data.render_mesh_data.model_matrix);
			shader->setMatrix("view", 1, Mat4(Mat3(camera.view)));
			shader->setMatrix("projection", 1, camera.projection);
			shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
			Renderer::drawIndex(*shader, render_skybox_sub_mesh_data.getVAO(), render_skybox_sub_mesh_data.indicesCount());
			shader->stop_using();
		}
	}

	// draw bright map to do bloom effect
	//m_gbuffer_framebuffer->blitDepthMapTo(m_lights_framebuffer.get());
	//for (auto entity : world.entityView<ecs::PointLightComponent>()) {
	//	auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
	//	auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
	//	auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

	//	for (int i = 0; i < renderable.sub_meshes.size(); i++) {
	//		auto& mesh = renderable.sub_meshes[i].mesh;
	//		auto& material = renderable.sub_meshes[i].material;
	//		Shader* shader = material->shader;
	//		material->update_shader_binding();
	//		shader->start_using();
	//		shader->setFloat4("color", point_light.luminousColor);
	//		shader->setMatrix("model", 1, model_matrix.transform());
	//		shader->setMatrix("view", 1, camera.view);
	//		shader->setMatrix("projection", 1, camera.projection);
	//		Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
	//		shader->stop_using();
	//	}
	//}
}

FrameBuffer* LightingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}

unsigned int LightingPass::getSceneMap() const
{
	return m_framebuffer->getAttachments()[0].getMap();
}

unsigned int LightingPass::getBrightMap() const
{
	return m_lights_framebuffer->getAttachments()[0].getMap();
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
	//auto& world = ecs::World::get();
	//ecs::CameraComponent& camera = *world.getMainCameraComponent();

	//Shader* normal_shader = Shader::getShader(ShaderType::NormalShader);
	//normal_shader->start_using();
	//normal_shader->setMatrix("view", 1, camera.view);
	//normal_shader->setMatrix("projection", 1, camera.projection);
	//normal_shader->setMatrix("projectionView", 1, camera.projection * camera.view);
	//for (auto entity : world.entityView<ecs::RenderableComponent>()) {
	//	auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
	//	auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
	//	normal_shader->setMatrix("model", 1, model_matrix.transform());
	//	for (int i = 0; i < renderable.sub_meshes.size(); i++) {
	//		auto& mesh = renderable.sub_meshes[i].mesh;
	//		Renderer::drawIndex(*normal_shader, mesh.get_VAO(), mesh.get_indices_count());
	//	}
	//}
}

void LightingPass::drawWireframeMode()
{
	//auto& world = ecs::World::get();
	//ecs::CameraComponent& camera = *world.getMainCameraComponent();

	//Shader* wireframe_shader = Shader::getShader(ShaderType::WireframeShader);
	//wireframe_shader->start_using();
	//wireframe_shader->setMatrix("view", 1, camera.view);
	//wireframe_shader->setMatrix("projection", 1, camera.projection);
	//for (auto entity : world.entityView<ecs::RenderableComponent>()) {
	//	auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
	//	auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
	//	wireframe_shader->setMatrix("model", 1, model_matrix.transform());
	//	for (int i = 0; i < renderable.sub_meshes.size(); i++) {
	//		auto& mesh = renderable.sub_meshes[i].mesh;
	//		Renderer::drawIndex(*wireframe_shader, mesh.get_VAO(), mesh.get_indices_count());
	//	}
	//}
}

void LightingPass::drawCheckerboardMode()
{
	//auto& world = ecs::World::get();
	//ecs::CameraComponent& camera = *world.getMainCameraComponent();
	//auto dir_light_component = world.getMainDirectionalLightComponent();

	//Mat4 light_ref_matrix = dir_light_component->lightReferenceMatrix();
	//Shader* shader = Shader::getShader(ShaderType::CheckerboardShader);
	//shader->start_using();
	//for (auto entity : world.entityView<ecs::RenderableComponent>()) {
	//	auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
	//	auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

	//	for (int i = 0; i < renderable.sub_meshes.size(); i++) {
	//		auto& mesh = renderable.sub_meshes[i].mesh;
	//		shader->setMatrix("modelScale", 1, Scale(model_matrix.scale));
	//		shader->setMatrix("model", 1, model_matrix.transform());
	//		shader->setMatrix("view", 1, camera.view);
	//		shader->setMatrix("projection", 1, camera.projection);
	//		Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
	//		shader->stop_using();
	//	}
	//}
}
