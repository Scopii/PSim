#include "StorageBuffer.h"

StorageBuffer::StorageBuffer(const void* data, unsigned int size, short layout, GLenum usage){
    drawType = usage;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, drawType);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, layout, m_RendererID);
}

StorageBuffer::~StorageBuffer(){
    glDeleteBuffers(1, &m_RendererID);
}

void StorageBuffer::Bind() const{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
}

void StorageBuffer::Unbind() const{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::Update(const void* data, unsigned int size) const{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, drawType);
}
