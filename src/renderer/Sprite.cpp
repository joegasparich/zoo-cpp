#include "Sprite.h"

#include <utility>

Sprite::Sprite(Texture *texture) : Sprite(texture, {0, 0}, {1, 1}) {}

Sprite::Sprite(Texture* texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds) :
    m_texture(texture),
    m_texCoord(texCoord),
    m_texBounds(texBounds) {}
