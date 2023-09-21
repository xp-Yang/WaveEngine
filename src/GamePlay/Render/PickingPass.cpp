#include "PickingPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Renderer.hpp"

void PickingPass::init()
{
    // ����picking֡����
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

            // TODO skybox ��ģ�;�����mat4(1.0f)��pickingҲ�ǰ������λ�ü�¼��
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
            // ע�����ﴫ��glReadPixels()���������������Ļ���½ǵ�
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

    for (auto entity : world.entityView<ecs::PickedComponent, ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

        // render border
        glClear(/*GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | */GL_STENCIL_BUFFER_BIT);
        // TODO ȥ�����еط���static Shader*
        static Shader* border_shader = new Shader("resource/shader/border.vs", "resource/shader/border.fs");

        glStencilFunc(GL_ALWAYS, 1, 0xFF); //����ͨ��ģ�����
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); //���ģ�����ͨ���ˣ�����ģ�建����Ϊ1
        glStencilMask(0xFF);

        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& mesh = renderable.primitives[i].mesh;
            auto& material = renderable.primitives[i].material;
            Shader* shader = material.shader; // TODO ��ô����const Shader *, ������û����
            shader->start_using();

            Renderer::drawIndex(*shader, mesh.get_VAO(), mesh.get_indices_count());
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
