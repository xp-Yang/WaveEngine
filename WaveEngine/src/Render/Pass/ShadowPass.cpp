#include "ShadowPass.hpp"

#include "Logical/Framework/SceneHierarchy.hpp"

void ShadowPass::init()
{
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	m_framebuffer->create({ AttachmentType::DEPTH });

    size_t max_point_light_count = SceneHierarchy::maxPointLightCount();
    reinit_cube_maps(max_point_light_count);

    glGenFramebuffers(1, &m_cube_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowPass::prepare(FrameBuffer* framebuffer)
{
}

void ShadowPass::configSamples(int samples)
{
    return;
    //TODO
    //config FrameBuffer
    m_framebuffer->bind();
    m_framebuffer->setSamples(samples);

    glBindTexture(GL_TEXTURE_2D, m_framebuffer->getFirstAttachmentOf(AttachmentType::DEPTH).getMap());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, samples * m_framebuffer->getWidth(), samples * m_framebuffer->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void ShadowPass::draw() {
    drawDirectionalLightShadowMap();
    drawPointLightShadowMap();
}

void ShadowPass::clear()
{
    m_framebuffer->bind();
    m_framebuffer->clear();
    m_framebuffer->unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    for(const auto& cube_map : m_cube_maps)
        for (int i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cube_map, 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer* ShadowPass::getFrameBuffer()
{
	return m_framebuffer.get();
}

void ShadowPass::drawDirectionalLightShadowMap()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    glEnable(GL_DEPTH_TEST);

    static RenderShaderObject* depth_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::DepthShader);
    depth_shader->start_using();
    Mat4 light_ref_matrix = m_render_source_data->render_directional_light_data_list.front().lightReferenceMatrix;
    for (const auto& render_sub_mesh_data : m_render_source_data->render_object_sub_mesh_data_list) {
        depth_shader->setMatrix("mvp", 1, light_ref_matrix * render_sub_mesh_data->transform());
        Renderer::drawIndex(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
    }
}

void ShadowPass::drawPointLightShadowMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);
    glViewport(0, 0, WINDOW_HEIGHT, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    static RenderShaderObject* depth_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::CubeMapShader);

    depth_shader->start_using();

    std::vector<std::array<Mat4, 6>> light_ref_matrix;
    std::vector<Vec3> light_pos;
    std::vector<float> light_radius;
    for (const auto& render_point_light_data : m_render_source_data->render_point_light_data_list) {
        light_ref_matrix.push_back(render_point_light_data.lightReferenceMatrix);
        light_pos.push_back(render_point_light_data.position);
        light_radius.push_back(render_point_light_data.radius);
    }

    if (m_render_source_data->render_point_light_data_list.size() > m_cube_maps.size()) {
        reinit_cube_maps(m_render_source_data->render_point_light_data_list.size());
    }

    for (int cube_map_id = 0; cube_map_id < m_render_source_data->render_point_light_data_list.size(); cube_map_id++) {
        for (int i = 0; i < 6; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cube_maps[cube_map_id], 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            for (const auto& render_sub_mesh_data : m_render_source_data->render_object_sub_mesh_data_list) {
                depth_shader->setMatrix("model", 1, render_sub_mesh_data->transform());
                depth_shader->setMatrix("lightSpaceMatrix", 1, light_ref_matrix[cube_map_id][i]);
                depth_shader->setFloat3("lightPos", light_pos[cube_map_id]);
                depth_shader->setFloat("far_plane", light_radius[cube_map_id]);
                Renderer::drawIndex(render_sub_mesh_data->getVAO(), render_sub_mesh_data->indicesCount());
            }
        }
    }
}

void ShadowPass::reinit_cube_maps(size_t count)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_cube_map_fbo);

    m_cube_maps.assign(count, 0);
    for (int i = 0; i < m_cube_maps.size(); i++) {
        glGenTextures(1, &m_cube_maps[i]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_maps[i]);
        for (GLuint i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WINDOW_HEIGHT, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}
