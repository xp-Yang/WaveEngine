#include "PickingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include <imgui/imgui.h>
#include "Application_impl.hpp"

void PickingPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGBA , AttachmentType::DEPTH});
}

void PickingPass::prepare(FrameBuffer* framebuffer)
{
}

void PickingPass::draw()
{
    // render for picking
    m_framebuffer->bind();
    m_framebuffer->clear();
    auto main_viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
    Application::GetApp().getWindow()->setMainViewport(main_viewport);

    glEnable(GL_DEPTH_TEST);

    Shader* picking_shader = Shader::getShader(ShaderType::PickingShader);
    picking_shader->start_using();

    auto& world = ecs::World::get();
    Mat4 camera_view = Mat4(1.0f);
    Mat4 camera_projection;
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    camera_view = camera.view;
    camera_projection = camera.projection;
    picking_shader->setMatrix("view", 1, camera_view);
    picking_shader->setMatrix("projection", 1, camera_projection);

    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if (world.hasComponent<ecs::UnpickableComponent>(entity))
            continue;
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
            Color4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
            picking_shader->setFloat4("picking_color", color);

            Renderer::drawIndex(*picking_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}

FrameBuffer* PickingPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
