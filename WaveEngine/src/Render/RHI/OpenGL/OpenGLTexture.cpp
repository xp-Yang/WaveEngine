#include "OpenGLTexture.hpp"

OpenGLTexture::OpenGLTexture(Format format_, const Vec2& pixelSize_, int sampleCount_, Flag flags_, unsigned char* data)
    : RhiTexture(format_, pixelSize_, sampleCount_, flags_, data)
{
}

bool OpenGLTexture::create()
{
    //unsigned int pbo;
    //glGenBuffers(1, &pbo);
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    //glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * nrComponents, 0, GL_STREAM_DRAW);
    //GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    //if (ptr)
    //{
    //    memcpy(ptr, data, width * height * nrComponents);
    //    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release the mapped buffer
    //}

    unsigned int textureID;
    switch (m_format)
    {
    case RhiTexture::Format::RGB16F: {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (int)m_pixelSize.x, (int)m_pixelSize.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }
    case RhiTexture::Format::RGBA32F:
    case RhiTexture::Format::RGBA16F: {
        if (m_sampleCount > 1) {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_RGBA, (int)m_pixelSize.x, (int)m_pixelSize.y, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)m_pixelSize.x, (int)m_pixelSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
            //使用GL_FLOAT和GL_UNSIGNED_BYTE区别
            //glTexImage2D 是旧接口，可以使用glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        break;
    }
    case RhiTexture::Format::DEPTH24STENCIL8: {
        // 统一用texture附件，不使用rbo附件
        //glGenRenderbuffers(1, &rbo);
        //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        //if (m_sampleCount > 1) {
        //    glGenTextures(1, &textureID);
        //    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
        //    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_DEPTH24_STENCIL8, (int)m_pixelSize.x, (int)m_pixelSize.y, GL_TRUE);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, textureID, 0);
        //}
        //else
        {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (int)m_pixelSize.x, (int)m_pixelSize.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        }
        break;
    }
    case RhiTexture::Format::DEPTH: {
        //if (m_sampleCount > 1) {
        //    glGenTextures(1, &textureID);
        //    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
        //    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_DEPTH_COMPONENT, (int)m_pixelSize.x, (int)m_pixelSize.y, GL_TRUE);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        //    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        //    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, textureID, 0);
        //}
        //else 
        {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)m_pixelSize.x, (int)m_pixelSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        }

        // if depth only:
        //不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
        break;
    }
    default:
        assert(false);
        break;
    }

    if (m_data)
    {
        GLenum format;
        if (m_format == R8 || m_format == R16 || m_format == R16F || m_format == R32F)
            format = GL_RED;
        else if (m_format = RGB16F)
            format = GL_RGB;
        else if (m_format == RGBA8 || m_format == RGBA16F || m_format == RGBA32F)
            format = GL_RGBA;
        else
            assert(false);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, (int)m_pixelSize.x, (int)m_pixelSize.y, 0, format, GL_UNSIGNED_BYTE, m_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
    }
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


    m_id = textureID;

    return true;
}
