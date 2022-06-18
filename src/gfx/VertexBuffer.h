#pragma once

#include "pch.h"
#include <cstddef>

class VertexBuffer {
private:
    unsigned int m_rendererID;
public:
    VertexBuffer(const void* data, size_t size, bool dynamic = false);
    ~VertexBuffer();

    void updateData(const void *data, size_t size, size_t offset);
    void bind() const;
    void unbind() const;
};
