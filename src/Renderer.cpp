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

// TODO ���ܲ���
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
    glm::mat4 light_ref_matrix;
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        auto& transform = *world.getComponent<ecs::TransformComponent>(entity);
        light_pos = transform.transform()[3];
        light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->light_reference_matrix();
    }

    for (auto entity : world.entityView<ecs::MeshComponent, ecs::MaterialComponent, ecs::TransformComponent>()) {
        auto& mesh = *world.getComponent<ecs::MeshComponent>(entity);
        auto& material = *world.getComponent<ecs::MaterialComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        for (int i = 0; i < mesh.meshes.size(); i++) {
            Shader* shader = material.materials[i].shader;
            material.materials[i].update_shader_binding();
            shader->start_using();
            auto model_mat = model_matrix.transform();
            if (world.hasComponent<ecs::SkyboxComponent>(entity)) {
                glDepthMask(GL_FALSE);
                shader->setMatrix("view", 1, skybox_view);
            }
            else
                shader->setMatrix("view", 1, camera_view);

            // TODO �ܲ�������÷���������Щshader����
            shader->setMatrix("model", 1, model_mat);
            shader->setMatrix("projection", 1, camera_projection);
            shader->setFloat3("camera_pos", camera_pos);
            shader->setFloat3("light.color", { 1.0f, 1.0f, 1.0f }); // TODO light_color
            shader->setFloat3("light.position", light_pos);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_id);
            shader->setTexture("skybox", 6);
            shader->setBool("enable_skybox_sample", /*obj->is_enable_reflection()*/false);
            shader->setFloat("magnitude", /*obj->get_explostion_ratio()*/0);
            if (view.is_shadow_map_enable()) {
                shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, view.get_shadow_map_id());
                shader->setTexture("shadow_map", 7);
            }
            drawIndex(*shader, mesh.meshes[i].get_VAO(), mesh.meshes[i].get_indices_count());
            glDepthMask(GL_TRUE);
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
    glEnable(GL_STENCIL_TEST);// Ϊ����Ⱦborder
    glStencilMask(0xFF);

    auto& world = ecs::World::get();

    glm::mat4 camera_view = glm::mat4(1.0f);
    glm::mat4 camera_projection;
    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        ecs::CameraComponent& camera = *world.getComponent<ecs::CameraComponent>(entity);
        camera_view = camera.view;
        camera_projection = camera.projection;
    }

    for (auto entity : world.entityView<ecs::PickedComponent, ecs::MeshComponent, ecs::MaterialComponent, ecs::TransformComponent>()) {
        auto& mesh = *world.getComponent<ecs::MeshComponent>(entity);
        auto& material = *world.getComponent<ecs::MaterialComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        auto model_mat = model_matrix.transform();

        // render border
        glClear(/*GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | */GL_STENCIL_BUFFER_BIT);
        // TODO ȥ�����еط���static Shader*
        static Shader* border_shader = new Shader("resource/shader/border.vs", "resource/shader/border.fs");

        glStencilFunc(GL_ALWAYS, 1, 0xFF); //����ͨ��ģ�����
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //���ģ�����ͨ���ˣ�����ģ�建����Ϊ1
        glStencilMask(0xFF);

        for (int i = 0; i < mesh.meshes.size(); i++) {
            Shader* shader = material.materials[i].shader; // TODO ��ô����const Shader *, ������û����
            shader->start_using();

            drawIndex(*shader, mesh.meshes[i].get_VAO(), mesh.meshes[i].get_indices_count());
            shader->stop_using();
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //ָ��ʲô�����ͨ��ģ����ԣ�����ֻ�е�ǰģ�建������Ϊ1�Ĳ��ֲ�ͨ������
        //API void glStencilFunc(GLenum func, GLint ref, GLuint mask);
        //funcͬ��Ȳ���һ����func ����Ϊ GL_NEVER��GL_LESS��GL_LEQUAL��GL_GREATER��GL_GEQUAL��GL_EQUAL��GL_NOTEQUAL��GL_ALWAYS��
        //ref�Ǻ͵�ǰģ�建���е�ֵstencil���бȽϵ�ָ��ֵ������ȽϷ�ʽʹ���˵���������mask������GL_LESSͨ�������ҽ���
        //���� : (ref & mask) < (stencil & mask).GL_GEQUALͨ�������ҽ���(ref & mask) >= (stencil & mask)��
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //ָ��ģ����Ժ���Ȳ���ͨ����ʧ��ʱִ�еĲ���������ʲô������
        //API void glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
        //sfail��ʾ��Ȳ���ʧ�ܣ�dpfail��ʾģ�����ͨ��������Ȳ���ʧ�ܣ�dppass��ʾģ����Ժ���Ȳ��Զ�ͨ��ʱ��ȡ����Ϊ��
        //�����Ƕ�Ӧ������ִ�еĶ���������GL_KEEP��ʾ������������ֵ��GL_REPLACE��ʾʹ��glStencilFunc���õ�refֵ�滻��
        glStencilMask(0x00);

        border_shader->start_using();
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.05f));
        border_shader->setMatrix("model", 1, model_mat * scale * glm::mat4(1.0f));
        border_shader->setMatrix("view", 1, camera_view);
        border_shader->setMatrix("projection", 1, camera_projection);
        for (int i = 0; i < mesh.meshes.size(); i++) {
            drawIndex(*border_shader, mesh.meshes[i].get_VAO(), mesh.meshes[i].get_indices_count());
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
    for (auto entity : world.entityView<ecs::MeshComponent, ecs::MaterialComponent, ecs::TransformComponent>()) {
        auto& mesh = *world.getComponent<ecs::MeshComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        auto model_mat = model_matrix.transform();

        normal_shader->start_using();
        normal_shader->setMatrix("model", 1, model_mat);
        for (int i = 0; i < mesh.meshes.size(); i++) {
            renderer.drawIndex(*normal_shader, mesh.meshes[i].get_VAO(), mesh.meshes[i].get_indices_count());
        }
    }
}

void Renderer::render_shadow_map(const View& view) {
    // TODO ��취�Ž�view����shadow_map���߼�
    auto& world = ecs::World::get();

    static Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    glm::mat4 light_ref_matrix;
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        light_ref_matrix = world.getComponent<ecs::LightComponent>(entity)->light_reference_matrix();
    }

    depth_shader->start_using();
    depth_shader->setMatrix("view", 1, light_ref_matrix);

    for (auto entity : world.entityView<ecs::MeshComponent, ecs::MaterialComponent, ecs::TransformComponent>()) {
        auto& mesh = *world.getComponent<ecs::MeshComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        auto model_mat = model_matrix.transform();

        depth_shader->start_using();
        depth_shader->setMatrix("model", 1, model_mat);
        for (int i = 0; i < mesh.meshes.size(); i++) {
            drawIndex(*depth_shader, mesh.meshes[i].get_VAO(), mesh.meshes[i].get_indices_count());
        }
    }
}