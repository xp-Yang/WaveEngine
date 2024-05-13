#include "GBufferPass.hpp"

void GBufferPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F,
        AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F,
        AttachmentType::DEPTH });
}

void GBufferPass::prepare(FrameBuffer* framebuffer)
{
}

void GBufferPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);

    static RenderShaderObject* g_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::GBufferShader);

    g_shader->start_using();
    g_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    g_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
    for (const auto& render_sub_mesh_data : m_render_source_data->render_object_sub_mesh_data_list) {
        g_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        auto& material = render_sub_mesh_data->renderMaterialData();
        g_shader->start_using();
        g_shader->setTexture("diffuse_map", 0, material.diffuse_map);
        g_shader->setTexture("specular_map", 1, material.specular_map);
        g_shader->setFloat3("albedo", material.albedo);
        g_shader->setFloat("metallic", material.metallic);
        g_shader->setFloat("roughness", material.roughness);
        g_shader->setFloat("ao", material.ao);
        Renderer::drawIndex(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
    g_shader->stop_using();
}

FrameBuffer* GBufferPass::getFrameBuffer()
{
    return m_framebuffer.get();
}