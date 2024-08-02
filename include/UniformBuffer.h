#pragma once
#include <glad/glad.h>

class UniformBuffer
{
private:
    unsigned int m_RendererID;
    GLenum drawType;
public:
    UniformBuffer(const void* data, unsigned int size, short index, GLenum usage);
    ~UniformBuffer();

    void Bind() const;
    void Unbind() const;
    void Update(const void* data, unsigned int size) const;

    unsigned int GetID() const { return m_RendererID; }
};