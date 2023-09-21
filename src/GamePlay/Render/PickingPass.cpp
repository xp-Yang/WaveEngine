#include "PickingPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Renderer.hpp"

void PickingPass::init()
{
    // 创建picking帧缓冲
	//glGenFramebuffers(1, &picking_FBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, picking_FBO);
	//view.set_picking_FBO(picking_FBO);
	//unsigned int picking_texture;
	//glActiveTexture(GL_TEXTURE8);
	//glGenTextures(1, &picking_texture);
	//glBindTexture(GL_TEXTURE_2D, picking_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);
	//unsigned int picking_depth_texture;
	//glGenTextures(1, &picking_depth_texture);
	//glActiveTexture(GL_TEXTURE7);
	//glBindTexture(GL_TEXTURE_2D, picking_depth_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, picking_depth_texture, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingPass::prepare_data(unsigned int fbo, unsigned int map)
{
}


void PickingPass::draw()
{
    // render for picking
    {
        static Shader* picking_shader = new Shader("resource/shader/picking.vs", "resource/shader/picking.fs");

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glBindFramebuffer(GL_FRAMEBUFFER, picking_FBO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        picking_shader->start_using();

        auto& world = ecs::World::get();
        glm::mat4 camera_view = glm::mat4(1.0f);
        glm::mat4 camera_projection;
        for (auto entity : world.entityView<ecs::CameraComponent>()) {
            ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
            camera_view = camera.view;
            camera_projection = camera.projection;
        }
        picking_shader->setMatrix("view", 1, camera_view);
        picking_shader->setMatrix("projection", 1, camera_projection);

        for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
            auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
            auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

            picking_shader->setMatrix("model", 1, model_matrix.transform());
            for (int i = 0; i < renderable.primitives.size(); i++) {
                auto& mesh = renderable.primitives[i].mesh;
                int id = entity.getId();
                int r = (id & 0x000000FF) >> 0;
                int g = (id & 0x0000FF00) >> 8;
                int b = (id & 0x00FF0000) >> 16;
                glm::vec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
                picking_shader->setFloat4("picking_color", color);

                Renderer::drawIndex(*picking_shader, mesh.get_VAO(), mesh.get_indices_count());
            }

            // TODO skybox 的模型矩阵是mat4(1.0f)，picking也是按照这个位置记录的
        }

        // Wait until all the pending drawing commands are really done.
        // Ultra-mega-over slow ! 
        // There are usually a long time between glDrawElements() and
        // all the fragments completely rasterized.
        if (ImGui::GetIO().MouseDown[0]) {
            glFlush();
            glFinish();

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            //glReadBuffer(GL_COLOR_ATTACHMENT0);
            unsigned char data[4] = { 0,0,0,0 };
            // 注意这里传给glReadPixels()的坐标是相对于屏幕左下角的
            int x = (int)ImGui::GetIO().MousePos.x;
            int y = (int)(WINDOW_HEIGHT - ImGui::GetIO().MousePos.y);
            glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);

            for (auto entity : world.entityView<ecs::RenderableComponent, ecs::TransformComponent>()) {
                if (entity.getId() == picked_id) {
                    world.addComponent<ecs::PickedComponent>(entity);
                }
                else
                    world.removeComponent<ecs::PickedComponent>(entity);
            }
        }

        //glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // render border
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

            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
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
            Renderer::drawIndex(*border_shader, mesh.get_VAO(), mesh.get_indices_count());
        }
        glStencilMask(0xFF);
    }
}
