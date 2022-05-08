#pragma once

#include "Texture.h"

class SubTexture {
public:
    SubTexture(std::shared_ptr<Texture> texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds);

    std::shared_ptr<Texture> m_texture;
    glm::vec2 m_texCoord;
    glm::vec2 m_texBounds;
};
