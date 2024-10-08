#include "TransparentPass.hpp"

TransparentPass::TransparentPass()
{
    m_type = RenderPass::Type::Transparent;
}

void TransparentPass::init()
{
}

void TransparentPass::draw()
{
    m_input_passes[0]->getFrameBuffer()->bind();

    auto shader_asset = Asset::Shader{ Asset::ShaderType::None, std::string(RESOURCE_DIR) + "/shader/mesh.vs", std::string(RESOURCE_DIR) + "/shader/transparent.fs"};
    static RenderShaderObject* shader = new RenderShaderObject(shader_asset);

    Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightProjMatrix *
        m_render_source_data->render_directional_light_data_list.front().lightViewMatrix;
    Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
    Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

    shader->start_using();
    shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
    for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
        const auto& render_sub_mesh_data = pair.second;
        if (render_sub_mesh_data->renderMaterialData().alpha == 1.0f)
            continue;

        shader->setMatrix("model", 1, render_sub_mesh_data->transform());

        auto& material = render_sub_mesh_data->renderMaterialData();

        shader->setTexture("material.diffuse_map", 0, material.diffuse_map);
        shader->setTexture("material.specular_map", 1, material.specular_map);
        shader->setTexture("material.normal_map", 2, material.normal_map);

        shader->setFloat3("cameraPos", m_render_source_data->camera_position);

        shader->setFloat3("directionalLight.direction", light_direction);
        shader->setFloat4("directionalLight.color", light_color);

        shader->setFloat("alpha", material.alpha);

        m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
    shader->stop_using();
}
