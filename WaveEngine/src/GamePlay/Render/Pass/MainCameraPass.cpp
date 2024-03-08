#include "MainCameraPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include <iostream>

void MainCameraPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    m_framebuffer->create({ AttachmentType::RGBA, AttachmentType::DEPTH24STENCIL8 });
}

void MainCameraPass::prepare(FrameBuffer* framebuffer)
{
    if (framebuffer)
        m_shadow_map = framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap();
    else
        m_shadow_map = 0;
}

void MainCameraPass::configShader(bool reflection, bool normal_debug, bool wireframe)
{
    //config shader 参数
    m_reflection = reflection;
    m_normal_debug = normal_debug;
    m_wireframe = wireframe;
}

void MainCameraPass::configSamples(int samples)
{
    //config FrameBuffer
    m_framebuffer->bind();
    m_framebuffer->setSamples(samples);
}

void MainCameraPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    auto& world = ecs::World::get();

    Mat4 camera_view = Mat4(1.0f);
    Mat4 camera_projection;
    Vec3 camera_pos;
    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    camera_view = camera.view;
    camera_projection = camera.projection;
    camera_pos = camera.pos;

    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
        if(world.hasComponent<ecs::SkyboxComponent>(entity))
            continue;
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
            // TODO 能不能想个好方法管理这些shader属性
            shader->start_using();
            shader->setMatrix("view", 1, camera_view);
            shader->setMatrix("model", 1, model_matrix.transform());
            shader->setMatrix("projection", 1, camera_projection);
            shader->setFloat3("cameraPos", camera_pos);

            auto dir_light_component = world.getMainDirectionalLightComponent();
            Mat4 light_ref_matrix = dir_light_component->lightReferenceMatrix();
            Vec3 light_direction = dir_light_component->direction;
            Vec4 light_color = dir_light_component->luminousColor;
            shader->setFloat3("directionalLight.direction", light_direction);
            shader->setFloat4("directionalLight.color", light_color);
            
            if (world.hasComponent<ecs::PointLightComponent>(entity)) {
                Vec4 light_color = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
                shader->setFloat4("color", light_color);
            }

            // 点光源这里的循环造成了卡顿，需要deferred rendering解决
            int k = 0;
            for (auto entity : world.entityView<ecs::PointLightComponent>()) {
                auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
                Vec3 light_pos = transform.transform()[3];
                Vec4 light_color = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
                float light_radius = world.getComponent<ecs::PointLightComponent>(entity)->radius;
                std::string light_id = std::string("pointLights[") + std::to_string(k) + "]";
                shader->setFloat3(light_id + ".position", light_pos);
                shader->setFloat4(light_id + ".color", light_color);
                shader->setFloat(light_id + ".radius", light_radius);
                k++;

                // 模拟正向渲染的嵌套for循环，性能明显下降
                //for (int i = 0; i < 45 /*primitives count*/; i++) {
                //	std::string light_id = std::string("lights[") + std::to_string(i) + "]";
                //}
                // 正向vs延迟渲染：
                // drawcall调用：  renderable.primitives.size()次  vs  gbufferPass：renderable.primitives.size()次 + LightingPass 1次。
                // for循环设置shader光源属性：  renderable.primitives.size() * Lights.size()次  vs  Lights.size()次。
                // shader计算：  每次drawcall，每个VAO绘制时，shader对光栅化后的片段并行计算，每个shader内部循环Lights.size()次
                //  vs  LightingPass的一次drawcall中，shader对整个屏幕所有像素并行计算，每个shader内部循环Lights.size()次。
            }
            shader->setInt("point_lights_size", k);

            shader->setCubeTexture("skybox", 6, world.getSkyboxComponent()->texture);
            shader->setBool("enable_skybox_sample", m_reflection);
            shader->setFloat("explosionRatio", explosion_ratio);
            if (m_shadow_map != 0) {
                shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
                shader->setTexture("shadow_map", 7, m_shadow_map);
            }
            else {
                // set default map
                //shader->setTexture("shadow_map", 7, 0);
            }
            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }
    }
    
    for (auto entity : world.entityView<ecs::SkyboxComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        auto skybox_texture_id = world.getComponent<ecs::SkyboxComponent>(entity)->texture;
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            Shader* shader = material.shader;
            material.update_shader_binding();
            shader->start_using();
            shader->setMatrix("model", 1, model_matrix.transform());
            shader->setMatrix("view", 1, Mat4(Mat3(camera_view)));
            shader->setMatrix("projection", 1, camera_projection);
            shader->setCubeTexture("skybox", 6, skybox_texture_id);
            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }
    }
    
    // render border,
    // TODO 1.模型坐标原点不是几何中心 2.深度测试和模板测试问题，挡住了wireframe和normal的显示
    //auto it = world.entityView<ecs::PickedComponent>();
    //if (it.begin() != it.end())
    //{
    //    glEnable(GL_STENCIL_TEST);
    //    glStencilMask(0xFF);

    //    auto& world = ecs::World::get();

    //    Mat4 camera_view = Mat4(1.0f);
    //    Mat4 camera_projection;
    //        ecs::CameraComponent& camera = *world.getMainCameraComponent();
    //        camera_view = camera.view;
    //        camera_projection = camera.projection;

    //    auto picked_entities = world.getPickedEntities();
    //    for (auto entity : picked_entities) {
    //        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
    //        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

    //        // render border
    //        glClear(GL_STENCIL_BUFFER_BIT);
    //        Shader* border_shader = Shader::getShader(ShaderType::BorderShader);

    //        glStencilFunc(GL_ALWAYS, 1, 0xFF); //总是通过模板测试
    //        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //如果模板测试通过了，设置模板缓冲区为1
    //        glStencilMask(0xFF);

    //        for (int i = 0; i < renderable.primitives.size(); i++) {
    //            auto& mesh = renderable.primitives[i].mesh;
    //            auto& material = renderable.primitives[i].material;
    //            Shader* shader = material.shader; // TODO 怎么不是const Shader *, 编译器没报错
    //            shader->start_using();

    //            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
    //            shader->stop_using();
    //        }

    //        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //指定什么情况下通过模板测试，这里只有当前模板缓冲区不为1的部分才通过测试
    //        //API void glStencilFunc(GLenum func, GLint ref, GLuint mask);
    //        //func同深度测试一样，func 参数为 GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL、GL_ALWAYS。
    //        //ref是和当前模板缓冲中的值stencil进行比较的指定值，这个比较方式使用了第三个参数mask，例如GL_LESS通过，当且仅当
    //        //满足 : (ref & mask) < (stencil & mask).GL_GEQUAL通过，当且仅当(ref & mask) >= (stencil & mask)。
    //        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //指定模板测试和深度测试通过或失败时执行的操作，这里什么都不做
    //        //API void glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
    //        //sfail表示深度测试失败，dpfail表示模板测试通过但是深度测试失败，dppass表示模板测试和深度测试都通过时采取的行为。
    //        //参数是对应条件下执行的动作，例如GL_KEEP表示保留缓冲区中值，GL_REPLACE表示使用glStencilFunc设置的ref值替换。
    //        glStencilMask(0x00);

    //        border_shader->start_using();
    //        auto scale = Scale(Vec3(1.05f));
    //        border_shader->setMatrix("model", 1, model_matrix.transform() * scale * Mat4(1.0f));
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
        Shader* normal_shader = Shader::getShader(ShaderType::NormalShader);

        auto& world = ecs::World::get();
        Mat4 camera_view;
        Mat4 camera_projection;
        ecs::CameraComponent& camera = *world.getMainCameraComponent();
        camera_view = camera.view;
        camera_projection = camera.projection;

        normal_shader->start_using();
        normal_shader->setMatrix("view", 1, camera_view);
        normal_shader->setMatrix("projection", 1, camera_projection);
        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
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
        Shader* wireframe_shader = Shader::getShader(ShaderType::WireframeShader);

        auto& world = ecs::World::get();
        Mat4 camera_view;
        Mat4 camera_projection;
        ecs::CameraComponent& camera = *world.getMainCameraComponent();
        camera_view = camera.view;
        camera_projection = camera.projection;

        wireframe_shader->start_using();
        wireframe_shader->setMatrix("view", 1, camera_view);
        wireframe_shader->setMatrix("projection", 1, camera_projection);
        for (auto entity : world.entityView<ecs::RenderableComponent>()) {
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

FrameBuffer* MainCameraPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
