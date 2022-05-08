#pragma once

#include "pch.h"
#include "Image.h"

class Texture {
public:
    Texture(Image* image);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;
    Image* m_image;
private:
    unsigned int m_rendererID;
};
