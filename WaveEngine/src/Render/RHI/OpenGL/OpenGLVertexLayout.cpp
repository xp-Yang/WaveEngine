#include "OpenGLVertexLayout.hpp"

OpenGLVertexLayout::OpenGLVertexLayout(RhiBuffer* vbuffer, RhiBuffer* ibuffer)
    : RhiVertexLayout(vbuffer, ibuffer)
{
}

bool OpenGLVertexLayout::create()
{
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer->id());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffer->id());
    for (int i = 0; i < m_attributes.size(); ++i) {
        int location = m_attributes[i].location;
        if (location == -1)
            continue;
        int stride = m_attributes[i].stride;
        int offset = m_attributes[i].offset;
        int size = 1;
        GLenum type = GL_FLOAT;
        switch (m_attributes[i].format) {
        case RhiVertexAttribute::Float4:
            type = GL_FLOAT;
            size = 4;
            break;
        case RhiVertexAttribute::Float3:
            type = GL_FLOAT;
            size = 3;
            break;
        case RhiVertexAttribute::Float2:
            type = GL_FLOAT;
            size = 2;
            break;
        case RhiVertexAttribute::Float:
            type = GL_FLOAT;
            size = 1;
            break;
        case RhiVertexAttribute::UInt4:
            type = GL_UNSIGNED_INT;
            size = 4;
            break;
        case RhiVertexAttribute::UInt3:
            type = GL_UNSIGNED_INT;
            size = 3;
            break;
        case RhiVertexAttribute::UInt2:
            type = GL_UNSIGNED_INT;
            size = 2;
            break;
        case RhiVertexAttribute::UInt:
            type = GL_UNSIGNED_INT;
            size = 1;
            break;
        case RhiVertexAttribute::SInt4:
            type = GL_INT;
            size = 4;
            break;
        case RhiVertexAttribute::SInt3:
            type = GL_INT;
            size = 3;
            break;
        case RhiVertexAttribute::SInt2:
            type = GL_INT;
            size = 2;
            break;
        case RhiVertexAttribute::SInt:
            type = GL_INT;
            size = 1;
            break;
        default:
            break;
        }
        glVertexAttribPointer(location, size, type, false, stride, (GLvoid*)(offset));
        glEnableVertexAttribArray(location);
    }
    return true;
}
