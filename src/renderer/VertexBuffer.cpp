#include <GL/glew.h>
#include <iostream>

#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
    GL_CALL(glGenBuffers(1, &m_rendererID));
    // Bind the buffer to let OpenGL know that this is the current buffer we're using
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
    // Load the positions into the buffer
    // Change this to GL_DYNAMIC_DRAW if we update the buffer often
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GL_CALL(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::bind() const {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::unbind() const {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
