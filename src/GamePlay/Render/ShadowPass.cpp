#include "ShadowPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

void ShadowPass::init()
{
	m_framebuffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	m_framebuffer->create({ AttachmentType::DEPTH });

    m_framebuffer_undersampled = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1);
    m_framebuffer_undersampled->create({ AttachmentType::DEPTH });
}

void ShadowPass::prepare(FrameBuffer* framebuffer)
{
}

void ShadowPass::configSamples(int samples)
{
    //config FrameBuffer
    m_framebuffer->bind();
    m_framebuffer->setSamples(samples);
}

void ShadowPass::draw() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);
    auto& world = ecs::World::get();

    static Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    glm::mat4 light_ref_matrix;
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->lightReferenceMatrix();
    }

    depth_shader->start_using();
    depth_shader->setMatrix("vp", 1, light_ref_matrix);

    for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
		if (world.hasComponent<ecs::SkyboxComponent>(entity) || world.hasComponent<ecs::LightComponent>(entity))
			continue;
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        depth_shader->start_using();
        depth_shader->setMatrix("model", 1, model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            Renderer::drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}

FrameBuffer* ShadowPass::getFrameBuffer()
{
    m_framebuffer->blitDepthMapTo(m_framebuffer_undersampled);
	return m_framebuffer_undersampled;
}
