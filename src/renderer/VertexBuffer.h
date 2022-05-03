#pragma once

#include <cstddef>

class VertexBuffer {
private:
    unsigned int m_rendererID;
public:
    VertexBuffer(const void* data, size_t size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
};
