#pragma once

class IndexBuffer {
private:
    unsigned int m_rendererID;
    unsigned int m_count;
public:
    IndexBuffer(const void* data, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    inline unsigned int getCount() const { return m_count; }
};
