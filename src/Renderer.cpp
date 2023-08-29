#include "Renderer.hpp"
#include "MySphere.hpp"

void Renderer::draw(const Shader& shader, GLuint vao_id, DrawMode mode, int indices_count, int array_count) const
{
    shader.start_using();

    glBindVertexArray(vao_id);

    if (mode == DrawMode::Arrays) {
        glDrawArrays(GL_TRIANGLES, 0, array_count);
    }
    else if (mode == DrawMode::Indices) {
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

void Renderer::render(const View& view) {
    auto& camera = view.get_camera();
    auto& skybox = view.get_scene().get_skybox();
    auto& light = view.get_scene().get_light();

    // skybox
    glDepthMask(GL_FALSE);
    Shader* skybox_shader = skybox.material().shader; // TODO 怎么不是const Shader *, 编译器没报错
    skybox.material().update_shader_binding();
    skybox_shader->start_using();
    auto skybox_view = camera.get_view();
    skybox_view = glm::mat4(glm::mat3(skybox_view));
    skybox_shader->setMatrix("model", 1, skybox.get_model_matrix());
    skybox_shader->setMatrix("view", 1, skybox_view);
    skybox_shader->setMatrix("projection", 1, camera.get_projection());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_texture_id());
    skybox_shader->setTexture("skybox", 6);
    draw(*skybox_shader, skybox.mesh().get_VAO(), DrawMode::Indices, skybox.mesh().get_indices_count());
    skybox_shader->stop_using();
    glDepthMask(GL_TRUE);

    // light
    Shader* light_shader = light.material().shader;
    light.material().update_shader_binding();
    light_shader->start_using();
    light_shader->setMatrix("model", 1, light.get_model_matrix());
    light_shader->setMatrix("view", 1, camera.get_view());
    light_shader->setMatrix("projection", 1, camera.get_projection());
    draw(*light_shader, light.mesh().get_VAO(), DrawMode::Indices, light.mesh().get_indices_count());
    light_shader->stop_using();

    glEnable(GL_STENCIL_TEST);// 为了渲染border
    glStencilMask(0xFF);
    // renderable objects
    for (auto& item : view.get_scene().get_objects()) {
        if (item.second && item.second->renderable()) {
            Object* obj = item.second;
            for (int i = 0; i < obj->get_meshes().size(); i++) {
                Shader* shader = obj->material(i).shader;
                obj->material(i).update_shader_binding();
                shader->start_using();
                shader->setMatrix("model", 1, obj->get_model_matrix());
                shader->setMatrix("view", 1, camera.get_view());
                shader->setMatrix("projection", 1, camera.get_projection());
                shader->setFloat3("camera_pos", camera.get_position());
                shader->setFloat3("light.color", light.material().color);
                shader->setFloat3("light.position", light.get_model_matrix()[3]);
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_texture_id());
                shader->setTexture("skybox", 6);
                shader->setBool("enable_skybox_sample", false);
                if (view.is_shadow_map_enable()) {
                    auto light_mat = light.get_light_space_matrix();
                    shader->setMatrix("lightSpaceMatrix", 1, light_mat);
                    // TODO 研究一下 这样为什么不行? shader->setMatrix("lightSpaceMatrix", 1, light.get_light_space_matrix());
                    glActiveTexture(GL_TEXTURE7);
                    glBindTexture(GL_TEXTURE_2D, view.get_shadow_map_id());
                    shader->setTexture("shadow_map", 7);
                }
                //glStencilFunc(GL_ALWAYS, 1, 0xFF); //所有片段都更新模板缓冲为1
                //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //如果模板测试和深度测试都通过了，设置模板缓冲区为1
                glBindVertexArray(obj->mesh(i).get_VAO());
                if (!obj->render_as_indices()) {
                    glDrawArrays(GL_TRIANGLES, 0, obj->mesh(i).get_vertices_count());
                }
                else {
                    glDrawElements(GL_TRIANGLES, obj->mesh(i).get_indices_count(), GL_UNSIGNED_INT, 0);
                }
                glBindVertexArray(0);
                shader->stop_using();
            }
            if (obj->is_picked()) {
                // render border
                glClear(/*GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | */GL_STENCIL_BUFFER_BIT);
                // TODO 去掉所有地方的static Shader*
                static Shader* border_shader = new Shader("resource/shader/border.vs", "resource/shader/border.fs");

                Shader* shader = obj->material().shader;
                shader->start_using();
                glStencilFunc(GL_ALWAYS, 1, 0xFF); //总是通过模板测试
                glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //如果模板测试通过了，设置模板缓冲区为1
                glStencilMask(0xFF);
                glBindVertexArray(obj->mesh().get_VAO());
                if (!obj->render_as_indices()) {
                    glDrawArrays(GL_TRIANGLES, 0, obj->mesh().get_vertices_count());
                }
                else {
                    glDrawElements(GL_TRIANGLES, obj->mesh().get_indices_count(), GL_UNSIGNED_INT, 0);
                }
                glBindVertexArray(0);
                shader->stop_using();

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
                auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.15f));
                border_shader->setMatrix("model", 1, obj->get_model_matrix() * scale * glm::mat4(1.0f));
                border_shader->setMatrix("view", 1, camera.get_view());
                border_shader->setMatrix("projection", 1, camera.get_projection());
                //draw(*border_shader, obj->mesh().get_VAO(), DrawMode::Indices, obj->mesh().get_indices_count());
                glBindVertexArray(obj->mesh().get_VAO());
                if (!obj->render_as_indices()) {
                    glDrawArrays(GL_TRIANGLES, 0, obj->mesh().get_vertices_count());
                }
                else {
                    glDrawElements(GL_TRIANGLES, obj->mesh().get_indices_count(), GL_UNSIGNED_INT, 0);
                }
                glBindVertexArray(0);
                glStencilMask(0xFF);
            }
        }
    }
}