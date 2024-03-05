#include "GBufferPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

void GBufferPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::RGB16F, AttachmentType::DEPTH });
}

void GBufferPass::prepare(FrameBuffer* framebuffer)
{
}

void GBufferPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);

    Shader* g_shader = Shader::getShader(ShaderType::GBufferShader);

    auto& world = ecs::World::get();

    Mat4 camera_view = Mat4(1.0f);
    Mat4 camera_projection;
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    camera_view = camera.view;
    camera_projection = camera.projection;

    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if (world.hasComponent<ecs::LightComponent>(entity) || world.hasComponent<ecs::BaseGridGroundComponent>(entity) || world.hasComponent<ecs::SkyboxComponent>(entity))
            continue;
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            material.update_shader_binding();
            g_shader->start_using();
            g_shader->setMatrix("view", 1, camera_view);
            g_shader->setMatrix("model", 1, model_matrix.transform());
            g_shader->setMatrix("projection", 1, camera_projection);
            g_shader->setTexture("diffuse_map", 11, material.diffuse_map);
            g_shader->setTexture("specular_map", 12, material.specular_map);
            Renderer::drawIndex(*g_shader, mesh.get_VAO(), mesh.get_indices_count());
            g_shader->stop_using();
        }
    }

}

FrameBuffer* GBufferPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
