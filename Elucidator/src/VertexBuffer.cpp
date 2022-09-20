#include "VertexBuffer.h"
#include "Utils.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW))
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}