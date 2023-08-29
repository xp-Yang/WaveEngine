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
        }
    }
}