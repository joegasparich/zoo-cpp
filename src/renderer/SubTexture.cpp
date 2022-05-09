#include "SubTexture.h"

#include <utility>

SubTexture::SubTexture(Texture* texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds) :
    m_texture(texture),
    m_texCoord(texCoord),
    m_texBounds(texBounds) {}
