#include "VertexArray.h"

VertexArray::VertexArray(){
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray(){
    Delete();
}

void VertexArray::Link(const VertexBuffer& VBO, GLuint layout, GLint numComponents, GLenum type, GLsizei stride, void* offset) {
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VertexArray::Bind() const{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const{
    glBindVertexArray(0);
}

void VertexArray::Delete() const{
    glDeleteVertexArrays(1, &m_RendererID);
}
