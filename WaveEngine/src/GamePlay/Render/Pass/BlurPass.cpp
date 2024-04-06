#include "BlurPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

BlurPass::BlurPass()
{
}

void BlurPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F });

    m_pingpong_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_pingpong_framebuffer->create({ AttachmentType::RGB16F });

    m_screen_quad = Mesh::create_screen_mesh();
}

void BlurPass::prepare(FrameBuffer* framebuffer)
{
    auto& world = ecs::World::get();
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    for (auto entity : world.entityView<ecs::PointLightComponent>()) {
        auto& point_light = *world.getComponent<ecs::PointLightComponent>(entity);
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            Shader* shader = material.shader;
            material.update_shader_binding();
            shader->start_using();
            shader->setFloat4("color", point_light.luminousColor);
            shader->setMatrix("model", 1, model_matrix.transform());
            shader->setMatrix("view", 1, camera.view);
            shader->setMatrix("projection", 1, camera.projection);
            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }
    }
}

void BlurPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_pingpong_framebuffer->bind();
    m_pingpong_framebuffer->clear();

    unsigned int map1 = m_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
    unsigned int map2 = m_pingpong_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();

    Shader* blur_shader = Shader::getShader(ShaderType::GaussianBlur);
    bool horizontal = true;
    unsigned int amount = 32;
    blur_shader->start_using();
    for (unsigned int i = 0; i < amount; i++)
    {
        if (i % 2 == 0) {
            m_framebuffer->bind();
            blur_shader->setTexture("image", 0, (i == 0) ? m_bright_map : map2);
        }
        else {
            m_pingpong_framebuffer->bind();
            blur_shader->setTexture("image", 0, (i == 0) ? m_bright_map : map1);
        }
        blur_shader->setInt("horizontal", horizontal);
        Renderer::drawIndex(*blur_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
        horizontal = !horizontal;
    }
    blur_shader->stop_using();
}

FrameBuffer* BlurPass::getFrameBuffer()
{
    return m_framebuffer.get();
}

unsigned int BlurPass::getBlurredBrightMap()
{
    return m_pingpong_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
}

void BlurPass::setBrightMap(unsigned int map)
{
    m_bright_map = map;
}
