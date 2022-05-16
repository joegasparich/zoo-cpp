#pragma once

class FrameBuffer {
public:
    FrameBuffer();
    virtual ~FrameBuffer();

    void bind();
    void unbind();

    unsigned int m_frameBufferID;
    unsigned int m_textureId;
    unsigned int m_renderBufferId;
private:
};
