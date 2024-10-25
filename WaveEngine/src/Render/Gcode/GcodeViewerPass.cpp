#include "GcodeViewerPass.hpp"

#include <glad/glad.h>// TODO remove

GcodeViewerPass::GcodeViewerPass()
{
}

void GcodeViewerPass::init()
{
}

void GcodeViewerPass::reload_mesh_data(std::array<LinesBatch, ExtrusionRole::erCount> lines_batches)
{
	for (int i = 0; i < m_VAOs.size(); i++) {
		// delete all the gl buffer
		glDeleteVertexArrays(1, &m_VAOs[i]);
		glDeleteBuffers(1, &m_VBOs[i]);
		glDeleteBuffers(1, &m_IBOs[i]);
	}
	m_VAOs.clear();
	m_VBOs.clear();
	m_IBOs.clear();
	m_colors.clear();
	for (int i = 0; i < lines_batches.size(); i++) {
		const auto& mesh = lines_batches[i].merged_mesh;
		if (!mesh || mesh->indices.empty()) {
			m_VAOs.push_back(0);
			m_VBOs.push_back(0);
			m_IBOs.push_back(0);
			m_colors.push_back({});
			continue;
		}

		unsigned int VBO = 0;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &(mesh->vertices[0]), GL_STATIC_DRAW);
		m_VBOs.push_back(VBO);

		unsigned int IBO = 0;
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ARRAY_BUFFER, IBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->indices.size() * sizeof(int), &(mesh->indices[0]), GL_DYNAMIC_DRAW);
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

		m_colors.push_back(Extrusion_Role_Colors[i]);
	}
}

void GcodeViewerPass::draw()
{
	m_input_passes[0]->getFrameBuffer()->bind();

	Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix *
		m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
	Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
	Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

	static RenderShaderObject* shader = RenderShaderObject::getShaderObject(ShaderType::GcodeShader);
	shader->start_using();
	shader->setMatrix("model", 1, Math::Rotate(-0.5 * Math::Constant::PI, Vec3(1, 0, 0)) * Math::Scale(Vec3(50.0f / 256.0f)) * Math::Translate(Vec3(-128.0f, -128.0f, 0.0f)));
	shader->setMatrix("view", 1, m_render_source_data->view_matrix);
	shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
	for (int i = 0; i < m_VAOs.size(); i++) {
		if (m_VAOs[i] == 0)
			continue;

		const auto& index_offset = m_gcode_viewer->colored_indices_interval(ExtrusionRole(i));
		int start_offset = index_offset.first;
		int size = index_offset.second - index_offset.first;

		shader->setFloat("material.ambient", 0.2f);
		shader->setFloat3("material.diffuse", Vec3(m_colors[i]));
		shader->setFloat3("material.specular", Vec3(0.2f));

		glBindVertexArray(m_VAOs[i]);
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (const void*)(start_offset * sizeof(int)));
		glBindVertexArray(0);

		{
			const auto& index_offset = m_gcode_viewer->colorless_indices_interval(ExtrusionRole(i));
			int start_offset = index_offset.first;
			int size = index_offset.second - index_offset.first;
			if (size > 0) {
				shader->setFloat3("material.diffuse", Vec3(Silent_Color));

				glBindVertexArray(m_VAOs[i]);
				glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (const void*)(start_offset * sizeof(int)));
				glBindVertexArray(0);
			}
		}
	}
	shader->stop_using();
}
