#include "ShadowPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

void ShadowPass::init()
{
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	m_framebuffer->create({ AttachmentType::DEPTH });


    glGenTextures(1, &m_cube_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
    for (GLuint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WINDOW_HEIGHT, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &m_cube_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_cube_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
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

void ShadowPass::clear()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_framebuffer->unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    for (int i = 0; i < 6; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cube_map, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glViewport(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    auto& world = ecs::World::get();
    Shader* depth_shader = Shader::getShader(ShaderType::CubeMapShader);

    depth_shader->start_using();

    std::vector<Mat4> light_ref_matrix;
    Vec3 light_pos;
    float light_radius;
    // TODO 只有一个点光源的情况
    for (auto entity : world.entityView<ecs::PointLightComponent>()) {
        auto point_light = world.getComponent<ecs::PointLightComponent>(entity);
        light_ref_matrix = point_light->lightReferenceMatrix();
        light_pos = point_light->position();
        light_radius = point_light->radius;
    }
    if (light_ref_matrix.empty())
        return;

    for (int i = 0; i < 6; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cube_map, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
            if (world.hasComponent<ecs::SkyboxComponent>(entity) || world.hasComponent<ecs::PointLightComponent>(entity))
                continue;
            auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
            auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

            depth_shader->setMatrix("model", 1, model_matrix.transform());
            depth_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix[i]);
            depth_shader->setFloat3("lightPos", light_pos);
            depth_shader->setFloat("far_plane", light_radius);
            for (int i = 0; i < renderable.primitives.size(); i++) {
                auto& mesh = renderable.primitives[i].mesh;
                Renderer::drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
            }
        }
    }

    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_cube_map, 0);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //for (auto entity : world.entityView<ecs::RenderableComponent>()) {
    //    if (world.hasComponent<ecs::SkyboxComponent>(entity) || world.hasComponent<ecs::PointLightComponent>(entity))
    //        continue;
    //    auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
    //    auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

    //    depth_shader->setMatrix("model", 1, model_matrix.transform());
    //    for (int i = 0; i < 6; i++) {
    //        depth_shader->setMatrix("lightSpaceMatrix[" + std::to_string(i) + "]", 1, light_ref_matrix[i]);
    //    }
    //    depth_shader->setFloat3("lightPos", light_pos);
    //    depth_shader->setFloat("far_plane", light_radius);
    //    for (int i = 0; i < renderable.primitives.size(); i++) {
    //        auto& mesh = renderable.primitives[i].mesh;
    //        Renderer::drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
    //    }
    //}
}
