#include "OpenGLBuffer.hpp"

OpenGLBuffer::OpenGLBuffer(Type type_, UsageFlag usage_, void* data, int size_)
    : RhiBuffer(type_, usage_, data, size_)
{
}

bool OpenGLBuffer::create()
{
    m_target_enum = GL_ARRAY_BUFFER;
    if (m_usage == RhiBuffer::UsageFlag::VertexBuffer)
        m_target_enum = GL_ARRAY_BUFFER;
    if (m_usage == RhiBuffer::UsageFlag::IndexBuffer)
        //https://stackoverflow.com/questions/20391921/binding-to-gl-element-array-buffer-with-no-vao-bound
        m_target_enum = GL_ARRAY_BUFFER; //GL_ELEMENT_ARRAY_BUFFER; 
    if (m_usage == RhiBuffer::UsageFlag::UniformBuffer)
        ;
    if (m_usage == RhiBuffer::UsageFlag::StorageBuffer)
        ;

    glGenBuffers(1, &m_id);
    glBindBuffer(m_target_enum, m_id);
    glBufferData(m_target_enum, m_size, m_data, /*m_type == Dynamic ? GL_DYNAMIC_DRAW : */GL_STATIC_DRAW);
    return true;
}
