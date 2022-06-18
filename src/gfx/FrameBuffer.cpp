#include "FrameBuffer.h"
#include "Renderer.h"

#include <GL/glew.h>

FrameBuffer::FrameBuffer() {
    GL_CALL(glGenFramebuffers(1, &m_frameBufferID));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));

    GL_CALL(glGenTextures(1, &m_textureId));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0));

    GL_CALL(glGenRenderbuffers(1, &m_renderBufferId));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferId));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT));
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferId));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to create framebuffer" << std::endl;
    }
}
FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::bind() {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
}

void FrameBuffer::unbind() {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
