#pragma once
#include <glad/glad.h>

class StorageBuffer
{
private:
    unsigned int m_RendererID;
    GLenum drawType;
public:
    StorageBuffer(const void* data, unsigned int size, short bindingNumber, GLenum usage);
    ~StorageBuffer();

    void Bind() const;
    void Unbind() const;
    void Update(const void* data, unsigned int size) const;

    unsigned int GetID() const { return m_RendererID; }
};
