#pragma once
#include <iostream>
#include <glad/glad.h>

//CURRENT MAX INDICES IS 256
class IndexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned char m_Count;

public:
    IndexBuffer(const GLubyte* data, unsigned char count, GLenum usage);
    ~IndexBuffer();
    void Bind() const;
    void Unbind() const;

    inline unsigned char GetCount() const { return m_Count; }
};