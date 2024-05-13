#include "MainCameraPass.hpp"

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

void MainCameraPass::configShader(bool skybox, bool reflection, bool normal_debug, bool wireframe)
{
    //config shader 参数
    m_skybox = skybox;
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

    Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightReferenceMatrix;
    Vec3 light_direction = m_render_source_data->render_directional_light_data_list.front().direction;
    Vec4 light_color = m_render_source_data->render_directional_light_data_list.front().color;

    static RenderShaderObject* shader = RenderShaderObject::getShaderObject(Asset::ShaderType::PBRShader);
    for (const auto& render_sub_mesh_data : m_render_source_data->render_object_sub_mesh_data_list) {
        auto& material = render_sub_mesh_data->renderMaterialData();
        shader->start_using();
        // temp
        shader->setFloat("material.shininess", material.shininess);
        shader->setFloat3("albedo", material.albedo);
        shader->setFloat("metallic", material.metallic);
        shader->setFloat("roughness", material.roughness);
        shader->setFloat("ao", material.ao);

        shader->setMatrix("model", 1, render_sub_mesh_data->transform());
        shader->setMatrix("view", 1, m_render_source_data->view_matrix);
        shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
        shader->setFloat3("cameraPos", m_render_source_data->camera_position);

        shader->setFloat3("directionalLight.direction", light_direction);
        shader->setFloat4("directionalLight.color", light_color);

        // 点光源这里的循环造成了卡顿，需要deferred rendering解决
        int k = 0;
        for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
            std::string light_id = std::string("pointLights[") + std::to_string(k) + "]";
            shader->setFloat3(light_id + ".position", render_point_light_data.position);
            shader->setFloat4(light_id + ".color", render_point_light_data.color);
            shader->setFloat(light_id + ".radius", render_point_light_data.radius);
            k++;
        }
        shader->setInt("point_lights_size", k);

        shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
        shader->setBool("enable_skybox_sample", m_reflection);
        if (m_shadow_map != 0) {
            shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix);
            shader->setTexture("shadow_map", 5, m_shadow_map);

            for (int i = 0; i < m_cube_maps.size(); i++) {
                std::string cube_map_id = std::string("cube_shadow_maps[") + std::to_string(i) + "]";
                shader->setCubeTexture(cube_map_id, 6 + i, m_cube_maps[i]);
            }
        }
        Renderer::drawIndex(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
        shader->stop_using();
    }
    
    static RenderShaderObject* skybox_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::SkyboxShader);
    if (m_skybox) {
        const auto& render_skybox_sub_mesh_data = m_render_source_data->render_skybox_data.render_sub_mesh_data;
        auto& material = render_skybox_sub_mesh_data->renderMaterialData();
        skybox_shader->start_using();
        skybox_shader->setMatrix("model", 1, render_skybox_sub_mesh_data->transform());
        skybox_shader->setMatrix("view", 1, Mat4(Mat3(m_render_source_data->view_matrix)));
        skybox_shader->setMatrix("projection", 1, m_render_source_data->proj_matrix);
        skybox_shader->setCubeTexture("skybox", 4, m_render_source_data->render_skybox_data.skybox_cube_map);
        Renderer::drawIndex(render_skybox_sub_mesh_data->getVAO(), render_skybox_sub_mesh_data->indicesCount());
        skybox_shader->stop_using();
    }

    // render border,
    // 1.模型坐标原点不是几何中心 2.深度测试和模板测试问题，挡住了wireframe和normal的显示
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

    //if (m_normal_debug)
    //{
    //    Shader* normal_shader = Shader::getShader(ShaderType::NormalShader);

    //    auto& world = ecs::World::get();
    //    Mat4 camera_view;
    //    Mat4 camera_projection;
    //    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    //    camera_view = camera.view;
    //    camera_projection = camera.projection;

    //    normal_shader->start_using();
    //    normal_shader->setMatrix("view", 1, camera_view);
    //    normal_shader->setMatrix("projection", 1, camera_projection);
    //    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
    //        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
    //        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

    //        normal_shader->start_using();
    //        normal_shader->setMatrix("model", 1, model_matrix.transform());
    //        for (int i = 0; i < renderable.sub_meshes.size(); i++) {
    //            auto& mesh = renderable.sub_meshes[i].mesh;
    //            Renderer::drawIndex(*normal_shader, mesh.get_VAO(), mesh.get_indices_count());
    //        }
    //    }
    //}

    //if (m_wireframe) {
    //    Shader* wireframe_shader = Shader::getShader(ShaderType::WireframeShader);

    //    auto& world = ecs::World::get();
    //    Mat4 camera_view;
    //    Mat4 camera_projection;
    //    ecs::CameraComponent& camera = *world.getMainCameraComponent();
    //    camera_view = camera.view;
    //    camera_projection = camera.projection;

    //    wireframe_shader->start_using();
    //    wireframe_shader->setMatrix("view", 1, camera_view);
    //    wireframe_shader->setMatrix("projection", 1, camera_projection);
    //    for (auto entity : world.entityView<ecs::RenderableComponent>()) {
    //        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
    //        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);

    //        wireframe_shader->start_using();
    //        wireframe_shader->setMatrix("model", 1, model_matrix.transform());
    //        for (int i = 0; i < renderable.sub_meshes.size(); i++) {
    //            auto& mesh = renderable.sub_meshes[i].mesh;
    //            Renderer::drawIndex(*wireframe_shader, mesh.get_VAO(), mesh.get_indices_count());
    //        }
    //    }
    //}
}

FrameBuffer* MainCameraPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
