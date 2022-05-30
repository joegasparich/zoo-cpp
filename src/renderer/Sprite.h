#pragma once

#include "Texture.h"

class Sprite {
public:
    Sprite(Texture* texture);
    Sprite(Texture* texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds);

    Texture* m_texture;
    glm::vec2 m_texCoord;
    glm::vec2 m_texBounds;
};
