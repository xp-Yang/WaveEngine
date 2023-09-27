#include "MainCameraPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

void MainCameraPass::init()
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    int samples_count = 16;
    if (samples_count == 1)
    {
        glGenTextures(1, &m_map);
        glBindTexture(GL_TEXTURE_2D, m_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_map, 0);
        // ΪʲôҪ������Ȼ������work? tex_buffer�����Ѿ�����Ȳ��Թ���һ��������
        // Ϊʲôֻ����stencil����Ӱ�ͻ������⣿
        unsigned int m_rbo;
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo); // now actually attach it
        //glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT); 
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
    }
    else {
        glGenTextures(1, &m_map);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_map);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_count, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_map, 0);
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_count, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainCameraPass::config(int msaa_sample_count, bool reflection, bool normal_debug, bool wireframe)
{
    m_msaa_sample_count = msaa_sample_count;
    m_reflection = reflection;
    m_normal_debug = normal_debug;
    m_wireframe = wireframe;
}

void MainCameraPass::prepare_data(unsigned int fbo, unsigned int map)
{
    m_shadow_map = map;
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_map);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_msaa_sample_count, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msaa_sample_count, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void MainCameraPass::draw()
{
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& world = ecs::World::get();

    glm::mat4 camera_view = glm::mat4(1.0f);
    glm::mat4 camera_projection;
    glm::vec3 camera_pos;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera_view = camera.view;
        camera_projection = camera.projection;
        camera_pos = camera.pos;
    }

    unsigned int skybox_texture_id = -1;
    auto skybox_view = camera_view;
    for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
        skybox_view = glm::mat4(glm::mat3(skybox_view));
        skybox_texture_id = world.getComponent<ecs::SkyboxComponent>(entity)->texture;
    }

    for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        float explosion_ratio = 0.0f;
        if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
            explosion_ratio = world.getComponent<ecs::ExplosionComponent>(entity)->explosionRatio;
        }

        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            Shader* shader = material.shader;
            material.update_shader_binding();
            shader->start_using();
            auto model_mat = model_matrix.transform();
            if (world.hasComponent<ecs::SkyboxComponent>(entity)) {
                glDepthMask(GL_FALSE);
                shader->setMatrix("view", 1, skybox_view);
            }
            else {
                glDepthMask(GL_TRUE);
                shader->setMatrix("view", 1, camera_view);
            }

            // TODO �ܲ�������÷���������Щshader����
            shader->setMatrix("model", 1, model_mat);
            shader->setMatrix("projection", 1, camera_projection);
            shader->setFloat3("camera_pos", camera_pos);
            glm::mat4 light_ref_matrix;
            int k = 0;
            for (auto entity : world.entityView<ecs::LightComponent>()) {
                auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
                glm::vec3 light_pos = transform.transform()[3];
                glm::vec4 light_color = world.getComponent<ecs::RenderableComponent>(entity)->primitives[0].material.color;
                light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->lightReferenceMatrix();
                std::string light_id = std::string("light[") + std::to_string(k) + "]";
                shader->setFloat4(light_id + ".color", light_color);
                shader->setFloat3(light_id + ".position", light_pos);
                k++;
            }
            shader->setCubeTexture("skybox", 6, skybox_texture_id);
            shader->setBool("enable_skybox_sample", m_reflection);
            shader->setFloat("explosionRatio", explosion_ratio);
            if (m_shadow_map != -1) {
                shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
                shader->setTexture("shadow_map", 7, m_shadow_map);
            }
            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }
    }

    // don't render border, because StencilMask causes problems
    //auto it = world.entityView<ecs::PickedComponent>();
    //if (it.begin() != it.end())
    //{
    //    // render border
    //    glEnable(GL_STENCIL_TEST);// Ϊ����Ⱦborder
    //    glStencilMask(0xFF);

    //    auto& world = ecs::World::get();

    //    glm::mat4 camera_view = glm::mat4(1.0f);
    //    glm::mat4 camera_projection;
    //    for (auto entity : world.entityView<ecs::CameraComponent>()) {
    //        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
    //        camera_view = camera.view;
    //        camera_projection = camera.projection;
    //    }

    //    for (auto entity : world.entityView<ecs::PickedComponent, ecs::RenderableComponent, ecs::TransformComponent>()) {
    //        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
    //        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

    //        // render border
    //        glClear(GL_STENCIL_BUFFER_BIT);
    //        // TODO ȥ�����еط���static Shader*
    //        static Shader* border_shader = new Shader("resource/shader/border.vs", "resource/shader/border.fs");

    //        glStencilFunc(GL_ALWAYS, 1, 0xFF); //����ͨ��ģ�����
    //        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //���ģ�����ͨ���ˣ�����ģ�建����Ϊ1
    //        glStencilMask(0xFF);

    //        for (int i = 0; i < renderable.primitives.size(); i++) {
    //            auto& mesh = renderable.primitives[i].mesh;
    //            auto& material = renderable.primitives[i].material;
    //            Shader* shader = material.shader; // TODO ��ô����const Shader *, ������û����
    //            shader->start_using();

    //            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
    //            shader->stop_using();
    //        }

    //        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //ָ��ʲô�����ͨ��ģ����ԣ�����ֻ�е�ǰģ�建������Ϊ1�Ĳ��ֲ�ͨ������
    //        //API void glStencilFunc(GLenum func, GLint ref, GLuint mask);
    //        //funcͬ��Ȳ���һ����func ����Ϊ GL_NEVER��GL_LESS��GL_LEQUAL��GL_GREATER��GL_GEQUAL��GL_EQUAL��GL_NOTEQUAL��GL_ALWAYS��
    //        //ref�Ǻ͵�ǰģ�建���е�ֵstencil���бȽϵ�ָ��ֵ������ȽϷ�ʽʹ���˵���������mask������GL_LESSͨ�������ҽ���
    //        //���� : (ref & mask) < (stencil & mask).GL_GEQUALͨ�������ҽ���(ref & mask) >= (stencil & mask)��
    //        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //ָ��ģ����Ժ���Ȳ���ͨ����ʧ��ʱִ�еĲ���������ʲô������
    //        //API void glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
    //        //sfail��ʾ��Ȳ���ʧ�ܣ�dpfail��ʾģ�����ͨ��������Ȳ���ʧ�ܣ�dppass��ʾģ����Ժ���Ȳ��Զ�ͨ��ʱ��ȡ����Ϊ��
    //        //�����Ƕ�Ӧ������ִ�еĶ���������GL_KEEP��ʾ������������ֵ��GL_REPLACE��ʾʹ��glStencilFunc���õ�refֵ�滻��
    //        glStencilMask(0x00);

    //        border_shader->start_using();
    //        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
    //        border_shader->setMatrix("model", 1, model_matrix.transform() * scale * glm::mat4(1.0f));
    //        border_shader->setMatrix("view", 1, camera_view);
    //        border_shader->setMatrix("projection", 1, camera_projection);
    //        for (int i = 0; i < renderable.primitives.size(); i++) {
    //            auto& mesh = renderable.primitives[i].mesh;
    //            Renderer::drawIndex(*border_shader, mesh.get_VAO(), mesh.get_indices_count());
    //        }
    //        glStencilMask(0xFF);
    //    }
    //}

    if (m_normal_debug)
    {
        static Shader* normal_shader = new Shader("resource/shader/model.vs", "resource/shader/normal.fs", "resource/shader/normal.gs");

        auto& world = ecs::World::get();
        glm::mat4 camera_view;
        glm::mat4 camera_projection;
        for (auto entity : world.entityView<ecs::CameraComponent>()) {
            ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
            camera_view = camera.view;
            camera_projection = camera.projection;
        }

        normal_shader->start_using();
        normal_shader->setMatrix("view", 1, camera_view);
        normal_shader->setMatrix("projection", 1, camera_projection);
        for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
            auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
            auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

            normal_shader->start_using();
            normal_shader->setMatrix("model", 1, model_matrix.transform());
            for (int i = 0; i < renderable.primitives.size(); i++) {
                auto& mesh = renderable.primitives[i].mesh;
                Renderer::drawIndex(*normal_shader, mesh.get_VAO(), mesh.get_indices_count());
            }
        }
    }

    if (m_wireframe) {
        static Shader* wireframe_shader = new Shader("resource/shader/model.vs", "resource/shader/wireframe.fs", "resource/shader/wireframe.gs");

        auto& world = ecs::World::get();
        glm::mat4 camera_view;
        glm::mat4 camera_projection;
        for (auto entity : world.entityView<ecs::CameraComponent>()) {
            ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
            camera_view = camera.view;
            camera_projection = camera.projection;
        }

        wireframe_shader->start_using();
        wireframe_shader->setMatrix("view", 1, camera_view);
        wireframe_shader->setMatrix("projection", 1, camera_projection);
        for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
            auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
            auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

            wireframe_shader->start_using();
            wireframe_shader->setMatrix("model", 1, model_matrix.transform());
            for (int i = 0; i < renderable.primitives.size(); i++) {
                auto& mesh = renderable.primitives[i].mesh;
                Renderer::drawIndex(*wireframe_shader, mesh.get_VAO(), mesh.get_indices_count());
            }
        }
    }
}
