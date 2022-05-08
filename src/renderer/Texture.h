#pragma once

#include "pch.h"
#include "Image.h"

class Texture {
public:
    Texture(Image* image);

    Texture(Image *image, const glm::vec2 &texCoord, const glm::vec2 &texBounds);

    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;
    Image* m_image;
    glm::vec2 m_texCoord;
    glm::vec2 m_texBounds;
private:
    unsigned int m_rendererID;
};
