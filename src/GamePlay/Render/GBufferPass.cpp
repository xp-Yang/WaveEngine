#include "GBufferPass.hpp"
#include "../ECS/Components.hpp"
#include "../../Platform/OpenGL/Renderer.hpp"
#include "../../Platform/OpenGL/rhi_opengl.hpp"

void GBufferPass::init()
{
    m_frame_buffer = new FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_frame_buffer->create({ AttachmentType::RGB, AttachmentType::RGB, AttachmentType::RGB });
    //// - 位置
    //glGenTextures(1, &m_position_map);
    //glBindTexture(GL_TEXTURE_2D, m_position_map);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_position_map, 0);
    //// - 法线
    //glGenTextures(1, &m_normal_map);
    //glBindTexture(GL_TEXTURE_2D, m_normal_map);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal_map, 0);
    //// - 颜色 + 镜面
    //glGenTextures(1, &m_color_spec_map);
    //glBindTexture(GL_TEXTURE_2D, m_color_spec_map);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_color_spec_map, 0);
    //// - 告诉OpenGL我们将要使用(帧缓冲的)哪种颜色附件来进行渲染
    //GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    //glDrawBuffers(3, attachments);
}

void GBufferPass::prepare(FrameBuffer* framebuffer)
{
}

void GBufferPass::draw()
{
    static Shader* gbuffer_shader = new Shader("resource/shader/gBuffer.vs", "resource/shader/gBuffer.fs");


}

FrameBuffer* GBufferPass::getFrameBuffer()
{
    return m_frame_buffer;
}
