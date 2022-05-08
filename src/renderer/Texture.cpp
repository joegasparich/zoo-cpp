#include "stb_image/stb_image.h"

#include "Renderer.h"
#include "Texture.h"

Texture::Texture(Image *image) :
    Texture(image, {0, 0}, {1.0, 1.0})
{}

Texture::Texture(Image* image, const glm::vec2 &texCoord, const glm::vec2 &texBounds)
    : m_rendererID{0},
      m_image{image},
      m_texCoord(texCoord),
      m_texBounds(texBounds)
{
    GL_CALL(glGenTextures(1, &m_rendererID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_rendererID));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    if (image->m_buffer) {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->m_width, image->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->m_buffer));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    } else {
        std::cout << "\nError: Failed to load texture " << image->m_filePath << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
}

Texture::~Texture() {
    GL_CALL(glDeleteTextures(1, &m_rendererID));
}

void Texture::bind(unsigned int slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_rendererID));
}

void Texture::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
