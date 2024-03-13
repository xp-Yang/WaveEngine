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
    return;
    //TODO
    //config FrameBuffer
    m_framebuffer->bind();
    m_framebuffer->setSamples(samples);

    glBindTexture(GL_TEXTURE_2D, m_framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, samples * m_framebuffer->getWidth(), samples * m_framebuffer->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void ShadowPass::draw() {
    drawDirectionalLightShadowMap();
    drawPointLightShadowMap();
}

FrameBuffer* ShadowPass::getFrameBuffer()
{
	return m_framebuffer.get();
}

void ShadowPass::drawDirectionalLightShadowMap()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);
    auto& world = ecs::World::get();

    Shader* depth_shader = Shader::getShader(ShaderType::DepthShader);

    Mat4 light_ref_matrix = world.getMainDirectionalLightComponent()->lightReferenceMatrix();

    depth_shader->start_using();

    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if (world.hasComponent<ecs::SkyboxComponent>(entity) || world.hasComponent<ecs::PointLightComponent>(entity))
            continue;
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        depth_shader->setMatrix("mvp", 1, light_ref_matrix * model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            Renderer::drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}

void ShadowPass::drawPointLightShadowMap()
{
    GLuint depthCubemap;
    glGenTextures(1, &depthCubemap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    glBindTexture(GL_TEXTURE_2D, depthCubemap);
    //for (GLuint i = 0; i < 6; ++i)
    //    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, /*GL_TEXTURE_CUBE_MAP_POSITIVE_X*/GL_TEXTURE_2D, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    auto& world = ecs::World::get();

    Shader* depth_shader = Shader::getShader(ShaderType::DepthShader);

    std::vector<Mat4> light_ref_matrix;

    for (auto entity : world.entityView<ecs::PointLightComponent>()) {
        light_ref_matrix = world.getComponent<ecs::PointLightComponent>(entity)->lightReferenceMatrix();
    }

    depth_shader->start_using();

    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if (world.hasComponent<ecs::SkyboxComponent>(entity) || world.hasComponent<ecs::PointLightComponent>(entity))
            continue;
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        depth_shader->setMatrix("mvp", 1, light_ref_matrix[3] * model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            Renderer::drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}
