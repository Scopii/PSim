#include "IndexBuffer.h"
//CURRENT MAX INDICES IS 256

IndexBuffer::IndexBuffer(const GLubyte* data, unsigned char count, GLenum usage)
    : m_Count(count)
{
    if (count > 256) {
        std::cerr << "Error: Index count exceeds 256!" << std::endl;
        return;
    }

    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLubyte), data, usage);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}