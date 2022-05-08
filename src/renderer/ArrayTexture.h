#pragma once

#include "pch.h"
#include "Image.h"

class ArrayTexture {
public:
    ArrayTexture(int width, int height, int maxLayers);
    virtual ~ArrayTexture();

    void pushTexture(const Image& image);

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const;
    int getHeight() const;

private:
    int m_width{}, m_height{};
    GLsizei m_layers{}, m_maxLayers{};
    unsigned int m_rendererID{};
};
