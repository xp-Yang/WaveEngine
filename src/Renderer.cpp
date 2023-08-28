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

void Renderer::render(const View& view) const {
    const auto& camera = view.get_camera();
    const auto& skybox = view.get_scene().get_skybox();
    const auto& light = view.get_scene().get_light();
    for (auto& item : view.get_scene().get_objects()) {
        if (item.second) {
            Object* obj = item.second;
            Shader* shader = obj->get_material().shader;
            shader->start_using();
            shader->setMatrix("model", 1, obj->get_model_matrix());
            shader->setMatrix("view", 1, camera.get_view());
            shader->setMatrix("projection", 1, camera.get_projection());
            shader->setFloat3("viewpos", camera.get_position());
            shader->setFloat3("light.color", light.get_material().color);
            shader->setFloat3("light.position", light.get_model_matrix()[3]);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_texture_id());
            shader->setTexture("skybox", 6);
            shader->setBool("enable_skybox_sample", true);
            //if (false) {
            //    glActiveTexture(GL_TEXTURE7);
            //    glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
            //    shader->setTexture("shadow_map", 7);
            //    shader->setMatrix("lightSpaceMatrix", 1, lightSpaceMatrix);
            //}
            glBindVertexArray(obj->get_mesh().get_VAO());
            if (obj->get_mesh().get_indices_count() <= 0) {
                // TODO refine
                auto sphere = dynamic_cast<MySphere*>(obj);
                if (sphere)
                    glDrawArrays(GL_TRIANGLES, 0, sphere->get_vertices_count());
            }
            else {
                glDrawElements(GL_TRIANGLES, obj->get_mesh().get_indices_count(), GL_UNSIGNED_INT, 0);
            }
            glBindVertexArray(0);
            shader->stop_using();
            // TODO model 这种有多个mesh的怎么办?
        }
    }
}