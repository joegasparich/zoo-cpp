#include "SubTexture.h"

#include <utility>

SubTexture::SubTexture(std::shared_ptr<Texture> texture, const glm::vec2 &texCoord, const glm::vec2 &texBounds) :
    m_texture(std::move(texture)),
    m_texCoord(texCoord),
    m_texBounds(texBounds) {}
