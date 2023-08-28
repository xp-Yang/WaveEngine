#include "View.hpp"

void View::enable_shadow_map(bool enable)
{
    //1.视角切换为光源视角，渲染到帧缓冲区，获得shadow_map
    Shader* depth_shader = new Shader("resource/shader/depth.vs", "resource/shader/depth.fs");

    auto& light = *const_cast<MyLight*>(&get_scene().get_light());

    //创建深度缓冲（阴影）
    unsigned int depth_frame_buffer;
    glGenFramebuffers(1, &depth_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
    unsigned int tex_depth_buffer;
    glGenTextures(1, &tex_depth_buffer);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, tex_depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth_buffer, 0);
    //不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::mat4 lightProjection = glm::ortho(-10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 10.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -5.0f, 15.0f, 0.1f, 100.0f);
    //lightProjection = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 light_pos = light.get_model_matrix()[3];
    glm::mat4 lightView = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    if (depth_shader) {
        depth_shader->start_using();
        depth_shader->setMatrix("view", 1, lightSpaceMatrix);
    }

    //2.执行默认缓冲区渲染，使用shadow_map
}

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