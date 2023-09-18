#include "Renderer.hpp"
#include "ECS/Components.hpp"
#include "ECS/System.hpp"
#include <windows.h>

void Renderer::drawIndex(const Shader& shader, GLuint vao_id, size_t indices_count) const
{
    shader.start_using();
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, (int)indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawTriangle(const Shader& shader, GLuint vao_id, size_t array_count) const
{
    shader.start_using();
    glBindVertexArray(vao_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)array_count);
    glBindVertexArray(0);
}

// TODO 性能测试
void Renderer::render_ecs(const View& view) 
{
    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);

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

    glm::vec3 light_pos;
    glm::vec4 light_color;
    glm::mat4 light_ref_matrix;
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
        light_pos = transform.transform()[3];
        light_color = world.getComponent<ecs::RenderableComponent>(entity)->primitives[0].material.color;
        light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->lightReferenceMatrix();
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

            // TODO 能不能想个好方法管理这些shader属性
            shader->setMatrix("model", 1, model_mat);
            shader->setMatrix("projection", 1, camera_projection);
            shader->setFloat3("camera_pos", camera_pos);
            shader->setFloat4("light.color", light_color);
            shader->setFloat3("light.position", light_pos);
            shader->setCubeTexture("skybox", 6, skybox_texture_id);
            shader->setBool("enable_skybox_sample", /*obj->is_enable_reflection()*/false);
            shader->setFloat("explosionRatio", explosion_ratio);
            if (view.is_shadow_map_enable()) {
                shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
                shader->setTexture("shadow_map", 7, view.get_shadow_map_id());
            }
            drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }
    }

    render_picking_border();

    QueryPerformanceCounter(&t2);
    auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    std::cout << "render_ecs(): time: " << time << std::endl;
}

void Renderer::render_picking_border()
{
    glEnable(GL_STENCIL_TEST);// 为了渲染border
    glStencilMask(0xFF);

    auto& world = ecs::World::get();

    glm::mat4 camera_view = glm::mat4(1.0f);
    glm::mat4 camera_projection;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera_view = camera.view;
        camera_projection = camera.projection;
    }

    for (auto entity : world.entityView<ecs::PickedComponent, ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        // render border
        glClear(/*GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | */GL_STENCIL_BUFFER_BIT);
        // TODO 去掉所有地方的static Shader*
        static Shader* border_shader = new Shader("resource/shader/border.vs", "resource/shader/border.fs");

        glStencilFunc(GL_ALWAYS, 1, 0xFF); //总是通过模板测试
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //如果模板测试通过了，设置模板缓冲区为1
        glStencilMask(0xFF);

        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            Shader* shader = material.shader; // TODO 怎么不是const Shader *, 编译器没报错
            shader->start_using();

            drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
            shader->stop_using();
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //指定什么情况下通过模板测试，这里只有当前模板缓冲区不为1的部分才通过测试
        //API void glStencilFunc(GLenum func, GLint ref, GLuint mask);
        //func同深度测试一样，func 参数为 GL_NEVER、GL_LESS、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL、GL_ALWAYS。
        //ref是和当前模板缓冲中的值stencil进行比较的指定值，这个比较方式使用了第三个参数mask，例如GL_LESS通过，当且仅当
        //满足 : (ref & mask) < (stencil & mask).GL_GEQUAL通过，当且仅当(ref & mask) >= (stencil & mask)。
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //指定模板测试和深度测试通过或失败时执行的操作，这里什么都不做
        //API void glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
        //sfail表示深度测试失败，dpfail表示模板测试通过但是深度测试失败，dppass表示模板测试和深度测试都通过时采取的行为。
        //参数是对应条件下执行的动作，例如GL_KEEP表示保留缓冲区中值，GL_REPLACE表示使用glStencilFunc设置的ref值替换。
        glStencilMask(0x00);

        border_shader->start_using();
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
        border_shader->setMatrix("model", 1, model_matrix.transform() * scale * glm::mat4(1.0f));
        border_shader->setMatrix("view", 1, camera_view);
        border_shader->setMatrix("projection", 1, camera_projection);
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            drawIndex(*border_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
        glStencilMask(0xFF);
    }
}

void Renderer::render_normal(const View& view) {
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
    Renderer renderer;
    for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        normal_shader->start_using();
        normal_shader->setMatrix("model", 1, model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            renderer.drawIndex(*normal_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}

void Renderer::render_shadow_map(const View& view) {
    glEnable(GL_DEPTH_TEST);
    // TODO 想办法放进view开启shadow_map的逻辑
    auto& world = ecs::World::get();

    static Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    glm::mat4 light_ref_matrix;
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->lightReferenceMatrix();
    }

    depth_shader->start_using();
    depth_shader->setMatrix("view", 1, light_ref_matrix);

    for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        depth_shader->start_using();
        depth_shader->setMatrix("model", 1, model_matrix.transform());
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            drawIndex(*depth_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
    }
}