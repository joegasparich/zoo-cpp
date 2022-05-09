#pragma once

#include "Texture.h"

class SubTexture {
public:
    SubTexture(Texture* texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds);

    Texture* m_texture;
    glm::vec2 m_texCoord;
    glm::vec2 m_texBounds;
};
