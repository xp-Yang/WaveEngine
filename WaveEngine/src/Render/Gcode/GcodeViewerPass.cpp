#include "GcodeViewerPass.hpp"

static Color4 IntToColor(int color) {
	float r = ((color >> 24) & 0x000000FF) / 255.0f;
	float g = ((color >> 16) & 0x000000FF) / 255.0f;
	float b = ((color >> 8) & 0x000000FF) / 255.0f;
	float a = ((color >> 0) & 0x000000FF) / 255.0f;
	return Color4(r, g, b, a);
}

GcodeViewerPass::GcodeViewerPass()
{
}

void GcodeViewerPass::init()
{
}

void GcodeViewerPass::draw()
{
	m_input_passes[0]->getFrameBuffer()->bind();

	Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix *
		m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
	Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
	Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

	static RenderShaderObject* shader = RenderShaderObject::getShaderObject(ShaderType::BlinnPhongShader);
	shader->start_using();

	int i = 0;
	for (const auto& mesh : m_render_source_data->gcode_render_mesh) {
		// temp
		shader->setFloat("material.ambient", 0.0f);
		//shader->setTexture("material.diffuse_map", 0, m_render_materials[i]->diffuse_map);
		//shader->setTexture("material.specular_map", 1, m_render_materials[i]->specular_map);
		//shader->setTexture("material.normal_map", 2, m_render_materials[i]->normal_map);
		//shader->setTexture("material.height_map", 3, m_render_materials[i]->height_map);
		shader->setFloat3("material.diffuse", m_render_source_data->gcode_render_materials[i]->albedo);
		shader->setFloat3("material.specular", Vec3(0.0f));

		shader->setMatrix("model", 1, Math::Rotate(-0.5 * Math::Constant::PI, Vec3(1, 0, 0)) * Math::Scale(Vec3(50.0f / 256.0f)) * Math::Translate(Vec3(-128.0f, -128.0f, 0.0f)));
		shader->setMatrix("view", 1, m_render_source_data->view_matrix);
		shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
		shader->setFloat3("cameraPos", m_render_source_data->camera_position);

		shader->setFloat3("directionalLight.direction", light_direction);
		shader->setFloat4("directionalLight.color", light_color);

		m_rhi->drawIndexed(mesh->getVAO(), mesh->indicesCount());
		i++;
	}
	shader->stop_using();
}
