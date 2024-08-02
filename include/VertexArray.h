#pragma once
#include <glad/glad.h>
#include "VertexBuffer.h"

class VertexArray
{
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    ~VertexArray();

    void Link(const VertexBuffer& VBO, GLuint layout, GLint numComponents, GLenum type, GLsizei stride, void* offset);
    void Bind() const;
    void Unbind() const;
    void Delete() const;

    unsigned int GetID() const { return m_RendererID; }
};
