#pragma once
#include <glad/glad.h>

class VertexBuffer
{
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size, GLenum usage);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    void Update(const void* data, unsigned int size) const;

    unsigned int GetID() const { return m_RendererID; }
};
