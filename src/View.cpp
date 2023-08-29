#include "View.hpp"

// TODO
//void View::enable_shadow_map(bool enable)
//{
//    //1.视角切换为光源视角，渲染到帧缓冲区，获得shadow_map
//    Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");
//
//    auto& light = *const_cast<MyLight*>(&get_scene().get_light());
//
//    //创建深度缓冲（阴影）
//    unsigned int depth_frame_buffer;
//    glGenFramebuffers(1, &depth_frame_buffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
//    unsigned int tex_depth_buffer;
//    glGenTextures(1, &tex_depth_buffer);
//    glActiveTexture(GL_TEXTURE7);
//    glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth_buffer, 0);
//    //不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
//    //glDrawBuffer(GL_NONE);
//    //glReadBuffer(GL_NONE);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    glm::mat4 lightProjection = glm::ortho(-10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -5.0f, 15.0f, 0.1f, 100.0f);
//    //lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
//    glm::vec3 light_pos = light.get_model_matrix()[3];
//    glm::mat4 lightView = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//    if (depth_shader) {
//        depth_shader->start_using();
//        depth_shader->setMatrix("view", 1, lightSpaceMatrix);
//    }
//
//    //2.执行默认缓冲区渲染，使用shadow_map
//}

void View::mouse_and_key_callback()
{
    ImGuiIO& io = ImGui::GetIO();
    auto& camera = get_camera();

    double xpos, ypos;
    xpos = io.MousePos.x;
    ypos = io.MousePos.y;
    if (io.WantCaptureMouse) {
        return;
    }
    static bool last_left_mouse_status = io.MouseDown[0];
    static bool last_right_mouse_status = io.MouseDown[1];

    static float last_pos_x = WINDOW_WIDTH / 2;
    static float last_pos_y = WINDOW_HEIGHT / 2;
    if (last_left_mouse_status != io.MouseDown[0]) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_left_mouse_status = io.MouseDown[0];
        if(io.MouseDown[0])
            render_picking();
    }
    if (io.MouseDown[0]) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y);
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 0);
    }

    if (last_right_mouse_status != io.MouseDown[1]) {
        last_pos_x = xpos;
        last_pos_y = ypos;
        last_right_mouse_status = io.MouseDown[1];
    }
    if (io.MouseDown[1]) {
        float delta_x = xpos - last_pos_x;
        float delta_y = -(ypos - last_pos_y);
        last_pos_x = xpos;
        last_pos_y = ypos;
        camera.mouse_process(delta_x, delta_y, 1);
    }

    camera.mouse_scroll_process(io.MouseWheel);

    float delta_time = 1.0f / io.Framerate;
    if (io.KeyShift)
        delta_time /= 10.0f;
    if (io.KeysDown['W']) {
        camera.key_process('W', delta_time);
    }
    if (io.KeysDown['A']) {
        camera.key_process('A', delta_time);
    }
    if (io.KeysDown['S']) {
        camera.key_process('S', delta_time);
    }
    if (io.KeysDown['D']) {
        camera.key_process('D', delta_time);
    }
}

void View::render_picking() {
    static Shader* picking_shader = new Shader("resource/shader/picking.vs", "resource/shader/picking.fs");

    auto& camera = get_camera();
    Skybox& skybox = *const_cast<Skybox*>(&get_scene().get_skybox());
    auto& light = *const_cast<MyLight*>(&get_scene().get_light());

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, picking_FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    picking_shader->start_using();
    picking_shader->setMatrix("view", 1, camera.get_view());
    picking_shader->setMatrix("projection", 1, camera.get_projection());

    {
        int id = skybox.get_id();
        int r = (id & 0x000000FF) >> 0;
        int g = (id & 0x0000FF00) >> 8;
        int b = (id & 0x00FF0000) >> 16;
        glm::vec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        picking_shader->setFloat4("picking_color", color);
        picking_shader->setMatrix("model", 1, skybox.get_model_matrix());
        glBindVertexArray(skybox.mesh().get_VAO());
        if (!skybox.render_as_indices()) {
            glDrawArrays(GL_TRIANGLES, 0, skybox.mesh().get_vertices_count());
        }
        else {
            glDrawElements(GL_TRIANGLES, skybox.mesh().get_indices_count(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    {
        int id = light.get_id();
        int r = (id & 0x000000FF) >> 0;
        int g = (id & 0x0000FF00) >> 8;
        int b = (id & 0x00FF0000) >> 16;
        glm::vec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        picking_shader->setFloat4("picking_color", color);
        picking_shader->setMatrix("model", 1, light.get_model_matrix());
        glBindVertexArray(light.mesh().get_VAO());
        if (!light.render_as_indices()) {
            glDrawArrays(GL_TRIANGLES, 0, light.mesh().get_vertices_count());
        }
        else {
            glDrawElements(GL_TRIANGLES, light.mesh().get_indices_count(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    for (auto& item : get_scene().get_objects()) {
        if (item.second && item.second->renderable()) {
            Object* obj = item.second;
            picking_shader->setMatrix("model", 1, obj->get_model_matrix());
            for (int i = 0; i < obj->get_meshes().size(); i++) {
                int id = obj->get_id();
                int r = (id & 0x000000FF) >> 0;
                int g = (id & 0x0000FF00) >> 8;
                int b = (id & 0x00FF0000) >> 16;
                glm::vec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
                picking_shader->setFloat4("picking_color", color);
                glBindVertexArray(obj->mesh(i).get_VAO());
                if (!obj->render_as_indices()) {
                    glDrawArrays(GL_TRIANGLES, 0, obj->mesh(i).get_vertices_count());
                }
                else {
                    glDrawElements(GL_TRIANGLES, obj->mesh(i).get_indices_count(), GL_UNSIGNED_INT, 0);
                }
                glBindVertexArray(0);
            }
        }
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
        int x = ImGui::GetIO().MousePos.x;
        int y = WINDOW_HEIGHT - ImGui::GetIO().MousePos.y;
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);
        for (auto& item : get_scene().get_objects()) {
            if (item.second) {
                if (item.second->get_id() == picked_id) {
                    item.second->set_picked(true);
                }
                else
                    item.second->set_picked(false);
            }
        }
    }

    //glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}