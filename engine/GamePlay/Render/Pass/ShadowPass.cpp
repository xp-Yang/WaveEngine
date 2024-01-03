#include "ShadowPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

void ShadowPass::init()
{
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	m_framebuffer->create({ AttachmentType::DEPTH });
}

void ShadowPass::prepare(FrameBuffer* framebuffer)
{
}

void ShadowPass::configSamples(int samples)
{
    //config FrameBuffer
    m_framebuffer->bind();
    m_framebuffer->setSamples(samples);

    glBindTexture(GL_TEXTURE_2D, m_framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, samples * m_framebuffer->getWidth(), samples * m_framebuffer->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void ShadowPass::draw() {
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
	return m_framebuffer.get();
}
