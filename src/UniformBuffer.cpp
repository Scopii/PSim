#include "../include/UniformBuffer.h"

UniformBuffer::UniformBuffer(const void* data, unsigned int size, short index, GLenum usage){
    drawType = usage;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, data, drawType);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_RendererID);
}

UniformBuffer::~UniformBuffer(){
    glDeleteBuffers(1, &m_RendererID);
}

void UniformBuffer::Bind() const{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void UniformBuffer::Unbind() const{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Update(const void* data, unsigned int size) const{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, data, drawType);
}
