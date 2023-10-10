#include "PickingPass.hpp"
#include "GamePlay/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include <imgui/imgui.h>

void PickingPass::init()
{
    m_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGBA , AttachmentType::DEPTH});
}

void PickingPass::prepare(FrameBuffer* framebuffer)
{
}

void PickingPass::draw()
{
    // render for picking
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);

    static Shader* picking_shader = new Shader("resource/shader/picking.vs", "resource/shader/picking.fs");
    picking_shader->start_using();

    auto& world = ecs::World::get();
    glm::mat4 camera_view = glm::mat4(1.0f);
    glm::mat4 camera_projection;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera_view = camera.view;
        camera_projection = camera.projection;
    }
    picking_shader->setMatrix("view", 1, camera_view);
    picking_shader->setMatrix("projection", 1, camera_projection);

    for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto name = world.getComponent<ecs::NameComponent>(entity);
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        picking_shader->setMatrix("model", 1, model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            int id = entity.getId() * 50000;// for debugging
            int r = (id & 0x000000FF) >> 0;
            int g = (id & 0x0000FF00) >> 8;
            int b = (id & 0x00FF0000) >> 16;
            glm::vec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
            picking_shader->setFloat4("picking_color", color);

            Renderer::drawIndex(*picking_shader, mesh.get_VAO(), mesh.get_indices_count());
        }

        // TODO skybox 的模型矩阵是mat4(1.0f)，picking也是按照这个位置记录的
    }

    // TODO 用事件处理
    // Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow ! 
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    if (ImGui::GetIO().MouseDown[0]) {
        glFlush();
        glFinish();

        // TODO: 含义？
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //glReadBuffer(GL_COLOR_ATTACHMENT0);
        unsigned char data[4] = { 0,0,0,0 };
        // 注意这里传给glReadPixels()的坐标是相对于屏幕左下角的
        int x = (int)ImGui::GetIO().MousePos.x;
        int y = (int)(WINDOW_HEIGHT - ImGui::GetIO().MousePos.y);
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);

        for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
            if (entity.getId() * 50000 == picked_id) {
                world.addComponent<ecs::PickedComponent>(entity);
            }
            else
                world.removeComponent<ecs::PickedComponent>(entity);
        }
        //glReadBuffer(GL_NONE);
    }
}

FrameBuffer* PickingPass::getFrameBuffer()
{
    return m_framebuffer;
}
