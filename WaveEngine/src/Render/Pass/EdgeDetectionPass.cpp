#include "EdgeDetectionPass.hpp"

EdgeDetectionPass::EdgeDetectionPass()
{
}

void EdgeDetectionPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F });

    m_source_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_source_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
}

void EdgeDetectionPass::prepare(FrameBuffer* framebuffer)
{
    m_source_framebuffer->bind();
    m_source_framebuffer->clear();

    //auto& world = ecs::World::get();
    //if (world.getPickedEntities().empty())
    //    return;
    //for (auto picked_entity : world.getPickedEntities()) {
        // render the picked one
        // TODO 需要先把实际的深度贴图拷贝到当前深度缓冲中
        glEnable(GL_DEPTH_TEST);

        static RenderShaderObject* picking_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::PickingShader);
        picking_shader->start_using();
        picking_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
        picking_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);

        // TODO 从render_mesh_data_list中查找到picked
        int i = 0;
        for (const auto& render_sub_mesh_data : m_render_source_data->render_object_sub_mesh_data_list) {
            picking_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
            int id = (i++) * 50000;// for debugging
            int r = (id & 0x000000FF) >> 0;
            int g = (id & 0x0000FF00) >> 8;
            int b = (id & 0x00FF0000) >> 16;
            Color4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
            picking_shader->setFloat4("picking_color", color);
            Renderer::drawIndex(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
        }
    //}
    m_source_map = m_source_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
}

void EdgeDetectionPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    static RenderShaderObject* edge_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::EdgeDetection);
    edge_shader->start_using();
    edge_shader->setTexture("Texture", 0, m_source_map);
    Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
}

FrameBuffer* EdgeDetectionPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
