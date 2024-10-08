#include "GBufferPass.hpp"

GBufferPass::GBufferPass()
{
    m_type = RenderPass::Type::GBuffer;
}

void GBufferPass::init()
{
    RhiTexture* color_texture0 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* color_texture1 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* color_texture2 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* color_texture3 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* color_texture4 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* color_texture5 = m_rhi->newTexture(RhiTexture::Format::RGB16F, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    RhiTexture* depth_texture = m_rhi->newTexture(RhiTexture::Format::DEPTH, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    color_texture0->create();
    color_texture1->create();
    color_texture2->create();
    color_texture3->create();
    color_texture4->create();
    color_texture5->create();
    depth_texture->create();
    RhiAttachment color_attachment = RhiAttachment(color_texture0);
    RhiAttachment depth_ttachment = RhiAttachment(depth_texture);
    RhiFrameBuffer* fb = m_rhi->newFrameBuffer(color_attachment, Vec2(DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y));
    fb->setColorAttachments({ color_texture0 , color_texture1 , color_texture2 , color_texture3 , color_texture4 ,
        color_texture5 });
    fb->setDepthAttachment(depth_ttachment);
    fb->create();
    m_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
}

void GBufferPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* g_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::GBufferShader);

    g_shader->start_using();
    g_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    g_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
    for (const auto& pair : m_render_source_data->render_mesh_data_hash) {
        const auto& render_sub_mesh_data = pair.second;
        if (render_sub_mesh_data->renderMaterialData().alpha != 1.0f)
            continue;

        g_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        auto& material = render_sub_mesh_data->renderMaterialData();
        if (m_pbr) {
            g_shader->setFloat3("albedo", material.albedo);
            g_shader->setFloat("metallic", material.metallic);
            g_shader->setFloat("roughness", material.roughness);
            g_shader->setFloat("ao", material.ao);
        }
        else {
            g_shader->setTexture("diffuse_map", 0, material.diffuse_map);
            g_shader->setTexture("specular_map", 1, material.specular_map);
        }
        m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
    g_shader->stop_using();

    m_framebuffer->unBind();
}

void GBufferPass::enablePBR(bool enable)
{
    m_pbr = enable;
}
