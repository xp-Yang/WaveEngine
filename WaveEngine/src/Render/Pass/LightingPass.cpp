#include "LightingPass.hpp"
// TODO remove
#include <glad/glad.h>
void LightingPass::init()
{
	RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
	RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
	color_texture->create();
	depth_texture->create();
	RhiAttachment color_attachment = RhiAttachment(color_texture);
	RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
	RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
	fb->setDepthAttachment(depth_ttachment);
	fb->create();
	m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);


	{
		RhiTexture* color_texture = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
		RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
		color_texture->create();
		depth_texture->create();
		RhiAttachment color_attachment = RhiAttachment(color_texture);
		RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
		RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
		fb->setDepthAttachment(depth_ttachment);
		fb->create();
		m_lights_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
	}
}

void LightingPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

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
	static RenderShaderObject* lighting_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::DeferredLightingShader);
	RhiFrameBuffer* gbuffer_framebuffer = m_input_passes[0]->getFrameBuffer();
	unsigned int g_position_map = gbuffer_framebuffer->colorAttachmentAt(0)->texture()->id();
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
	// debug option
	lighting_shader->setBool("enablePBR", m_pbr);

	RhiFrameBuffer* shadow_framebuffer = m_input_passes[1]->getFrameBuffer();
	m_dir_light_shadow_map = shadow_framebuffer->depthAttachment()->texture()->id();
	lighting_shader->setFloat3("cameraPos", m_render_source_data->camera_position);
	for (const auto& render_directional_light_data : m_render_source_data->render_directional_light_data_list) {
		lighting_shader->setFloat3("directionalLight.direction", render_directional_light_data.direction);
		lighting_shader->setFloat4("directionalLight.color", render_directional_light_data.color);
		if (m_dir_light_shadow_map != 0) {
			lighting_shader->setMatrix("lightSpaceMatrix", 1, render_directional_light_data.lightReferenceMatrix);
			lighting_shader->setTexture("shadow_map", 8, m_dir_light_shadow_map);
		}
	}
	for (int i = 0; i < m_cube_maps.size(); i++) {
		std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
		lighting_shader->setCubeTexture(cube_map_id, 9 + i, m_cube_maps[i]);
	}
	int point_light_idx = 0;
	for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
		std::string light_id = std::string("pointLights[") + std::to_string(point_light_idx) + "]";
		lighting_shader->setFloat3(light_id + ".position", render_point_light_data.position);
		lighting_shader->setFloat4(light_id + ".color", render_point_light_data.color);
		lighting_shader->setFloat(light_id + ".radius", render_point_light_data.radius);
		point_light_idx++;
	}
	lighting_shader->setInt("point_lights_size", m_render_source_data->render_point_light_data_list.size());
	m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	lighting_shader->stop_using();


	gbuffer_framebuffer->blitTo(m_framebuffer.get(), RhiTexture::Format::DEPTH);

	// TODO lights直接instancing rendering
	// lights
	static Asset::Shader point_light_shader_asset { Asset::ShaderType::CustomShader, std::string(RESOURCE_DIR) + "/shader/light.vs", std::string(RESOURCE_DIR) + "/shader/light.fs" };
	static RenderShaderObject* point_light_shader = new RenderShaderObject(point_light_shader_asset);
	for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
		const auto& render_point_light_sub_mesh_data = render_point_light_data.render_sub_mesh_data;
		auto& material = render_point_light_sub_mesh_data->renderMaterialData();
		point_light_shader->start_using();
		point_light_shader->setFloat4("color", render_point_light_data.color);
		point_light_shader->setMatrix("model", 1, render_point_light_sub_mesh_data->transform());
		point_light_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
		point_light_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
		m_rhi->drawIndexed(render_point_light_sub_mesh_data->getVAO(), render_point_light_sub_mesh_data->indicesCount());
		point_light_shader->stop_using();
	}

	// normal
	if (m_normal_debug) {
		drawNormalMode();
	}

	// skybox
	static RenderShaderObject* skybox_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::SkyboxShader);
	if (m_skybox) {
		const auto& render_skybox_sub_mesh_data = m_render_source_data->render_skybox_data.render_sub_mesh_data;
		auto& material = render_skybox_sub_mesh_data->renderMaterialData();
		skybox_shader->start_using();
		skybox_shader->setMatrix("model", 1, render_skybox_sub_mesh_data->transform());
		skybox_shader->setMatrix("view", 1, Mat4(Mat3(m_render_source_data->view_matrix)));
		skybox_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
		skybox_shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
		m_rhi->drawIndexed(render_skybox_sub_mesh_data->getVAO(), render_skybox_sub_mesh_data->indicesCount());
		skybox_shader->stop_using();
	}

	// TODO light亮度加强，bloom直接提取亮度超出阈值的部分
	// draw bright map to do bloom effect
	//m_lights_framebuffer->bind();
	//m_lights_framebuffer->clear();
	//m_gbuffer_framebuffer->blitDepthMapTo(m_lights_framebuffer.get());
	//for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
	//	const auto& render_point_light_sub_mesh_data = render_point_light_data.render_sub_mesh_data;
	//	auto& material = render_point_light_sub_mesh_data->renderMaterialData();
	//	point_light_shader->start_using();
	//	point_light_shader->setFloat4("color", render_point_light_data.color);
	//	point_light_shader->setMatrix("model", 1, render_point_light_sub_mesh_data->transform());
	//	point_light_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	//	point_light_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	//	m_rhi->drawIndexed(render_point_light_sub_mesh_data->getVAO(), render_point_light_sub_mesh_data->indicesCount());
	//	point_light_shader->stop_using();
	//}

	m_framebuffer->unBind();
}

unsigned int LightingPass::getSceneMap() const
{
	return m_framebuffer->colorAttachmentAt(0)->texture()->id();
}

unsigned int LightingPass::getBrightMap() const
{
	return m_lights_framebuffer->colorAttachmentAt(0)->texture()->id();
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
	static RenderShaderObject* normal_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::NormalShader);
	normal_shader->start_using();
	normal_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	normal_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	normal_shader->setMatrix("projectionView", 1, m_render_source_data->proj_matrix * m_render_source_data->view_matrix);
	for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
		const auto& render_sub_mesh_data = pair.second;
		normal_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
		m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
	}
}

void LightingPass::drawWireframeMode()
{
	static RenderShaderObject* wireframe_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::WireframeShader);
	wireframe_shader->start_using();
	wireframe_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	wireframe_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
		const auto& render_sub_mesh_data = pair.second;
		wireframe_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
		m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
	}
}

void LightingPass::drawCheckerboardMode()
{
	static RenderShaderObject* shader = RenderShaderObject::getShaderObject(Asset::ShaderType::CheckerboardShader);
	shader->start_using();
	shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
		const auto& render_sub_mesh_data = pair.second;
		shader->setMatrix("modelScale", 1, Math::Scale(Vec3(1.0f))); // TODO get scale of model_matrix
		shader->setMatrix("model", 1, render_sub_mesh_data->transform());
		m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
	}
	shader->stop_using();
}
