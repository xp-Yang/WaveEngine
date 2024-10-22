#include "GcodeViewerPass.hpp"

#include <glad/glad.h>// TODO remove

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

void GcodeViewerPass::reload_mesh_data(std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount> gcode_meshes)
{
	m_VAOs.clear();
	m_VBOs.clear();
	m_IBOs.clear();
	for (int i = 0; i < gcode_meshes.size(); i++) {
		const auto& mesh = gcode_meshes[i];
		if (!mesh || mesh->indices.empty()) {
			m_VAOs.push_back(0);
			m_VBOs.push_back(0);
			m_IBOs.push_back(0);
			continue;
		}

		unsigned int VBO = 0;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &(mesh->vertices[0]), GL_STATIC_DRAW);
		m_VBOs.push_back(VBO);

		unsigned int IBO = 0;
		glGenBuffers(1, &IBO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data->indices().size() * sizeof(unsigned int), &(mesh_data->indices()[0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, IBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &(mesh->indices[0]), GL_DYNAMIC_DRAW);
		m_IBOs.push_back(IBO);

		unsigned int VAO = 0;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
		glEnableVertexAttribArray(2);
		m_VAOs.push_back(VAO);
	}
}

void GcodeViewerPass::draw()
{
	if (m_gcode_viewer->dirty()) {
		for (int i = 0; i < m_gcode_viewer->meshes().size(); i++) {
			const auto& mesh = m_gcode_viewer->meshes()[i];
			if (!mesh || mesh->indices.empty())
				continue;

			glBindBuffer(GL_ARRAY_BUFFER, m_IBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &(mesh->indices[0]), GL_DYNAMIC_DRAW);
		}
		m_gcode_viewer->setDirty(false);
	}

	m_input_passes[0]->getFrameBuffer()->bind();

	Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix *
		m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
	Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
	Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

	static RenderShaderObject* shader = RenderShaderObject::getShaderObject(ShaderType::BlinnPhongShader);
	shader->start_using();
	shader->setInt("point_lights_size", 0);
	shader->setFloat3("directionalLight.direction", light_direction);
	shader->setFloat4("directionalLight.color", light_color);
	shader->setMatrix("model", 1, Math::Rotate(-0.5 * Math::Constant::PI, Vec3(1, 0, 0)) * Math::Scale(Vec3(50.0f / 256.0f)) * Math::Translate(Vec3(-128.0f, -128.0f, 0.0f)));
	shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	shader->setFloat3("cameraPos", m_render_source_data->camera_position);
	for (int i = 0; i < m_gcode_viewer->meshes().size(); i++) {
		const auto& mesh = m_gcode_viewer->meshes()[i];
		if (!mesh || mesh->indices.empty())
			continue;
		if (!m_gcode_viewer->is_visible(ExtrusionRole(i)))
			continue;

		shader->setFloat("material.ambient", 0.2f);
		shader->setFloat3("material.diffuse", mesh->material->albedo);
		shader->setFloat3("material.specular", Vec3(0.0f));

		m_rhi->drawIndexed(m_VAOs[i], mesh->indices.size());
	}
	shader->stop_using();
}
