#include <GL/glew.h>
#include <iostream>

#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void *data, size_t size, bool dynamic) {
    GL_CALL(glGenBuffers(1, &m_rendererID));
    // Bind the buffer to let OpenGL know that this is the current buffer we're using
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
    // Load the positions into the buffer
    // Change this to GL_DYNAMIC_DRAW if we update the buffer often
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GL_CALL(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::updateData(const void *data, size_t size, size_t offset) {
    bind();
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void VertexBuffer::bind() const {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::unbind() const {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
