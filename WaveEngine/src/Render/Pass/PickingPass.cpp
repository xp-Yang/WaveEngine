#include "PickingPass.hpp"

void PickingPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F , AttachmentType::DEPTH});
}

void PickingPass::prepare(FrameBuffer* framebuffer)
{
}

void PickingPass::draw()
{
    // render for picking
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);

    Shader* picking_shader = Shader::getShader(ShaderType::PickingShader);
    picking_shader->start_using();

    picking_shader->setMatrix("view", 1, m_render_source_data->view_matrix);
    picking_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);

    // TODO Unpickable
    int i = 0;
    for (const auto& render_mesh_data : m_render_source_data->render_mesh_data_list) {
        picking_shader->setMatrix("model", 1, render_mesh_data.model_matrix);
        for (const auto& render_sub_mesh_data : render_mesh_data.render_sub_mesh_data_list) {
            int id = i++ * 50000;// for debugging
            int r = (id & 0x000000FF) >> 0;
            int g = (id & 0x0000FF00) >> 8;
            int b = (id & 0x00FF0000) >> 16;
            Color4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
            picking_shader->setFloat4("picking_color", color);
            Renderer::drawIndex(*picking_shader, render_sub_mesh_data.getVAO(), render_sub_mesh_data.indicesCount());
        }
    }
}

FrameBuffer* PickingPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
