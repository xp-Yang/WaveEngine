#include "EdgeDetectionPass.hpp"

EdgeDetectionPass::EdgeDetectionPass()
{
}

void EdgeDetectionPass::init()
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
        m_source_framebuffer = std::unique_ptr<RhiFrameBuffer>(fb);
    }
}

void EdgeDetectionPass::draw()
{
    {
        m_source_framebuffer->bind();
        m_source_framebuffer->clear();

        if (m_render_source_data->picked_ids.empty()) {
            m_framebuffer->bind();
            m_framebuffer->clear();
            return;
        }
        for (auto picked_id : m_render_source_data->picked_ids) {
            // render the picked one
            static RenderShaderObject* picking_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::OneColorShader);
            picking_shader->start_using();
            picking_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
            picking_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);

            auto it = std::find_if(m_render_source_data->render_mesh_data_hash.begin(), m_render_source_data->render_mesh_data_hash.end(),
                [picked_id](const std::pair<const RenderMeshDataID, std::shared_ptr<RenderMeshData>>& pair) {
                    return pair.second->ID().object_id == picked_id;
                }
            );
            if (it != m_render_source_data->render_mesh_data_hash.end()) {
                const auto& render_sub_mesh_data = it->second;
                picking_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
                int id = picked_id;
                int r = (id & 0x000000FF) >> 0;
                int g = (id & 0x0000FF00) >> 8;
                int b = (id & 0x00FF0000) >> 16;
                Color4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
                picking_shader->setFloat4("color", color);
                m_rhi->drawIndexed(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
            }
        }
        m_source_map = m_source_framebuffer->colorAttachmentAt(0)->texture()->id();
    }

    m_framebuffer->bind();
    m_framebuffer->clear();
    m_input_passes[0]->getFrameBuffer()->blitTo(m_framebuffer.get(), RhiTexture::Format::DEPTH);
    auto obj_depth_map = m_source_framebuffer->depthAttachment()->texture()->id();
    static RenderShaderObject* edge_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::EdgeDetection);
    edge_shader->start_using();
    edge_shader->setTexture("objMap", 0, m_source_map);
    edge_shader->setTexture("objDepthMap", 1, obj_depth_map);
    m_rhi->drawIndexed(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
}
