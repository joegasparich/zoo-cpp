#include "lib/stb_image/stb_image.h"

#include "Renderer.h"
#include "Texture.h"

Texture::Texture(const std::string &path)
    : m_rendererID{0},
      m_filePath{path},
      m_localBuffer{nullptr},
      m_width{0},
      m_height{0},
      m_bpp{0}
{
    stbi_set_flip_vertically_on_load(0);
    m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 4);

    GL_CALL(glGenTextures(1, &m_rendererID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_rendererID));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    if (m_localBuffer) {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        stbi_image_free(m_localBuffer);
    } else {
        std::cout << "\nError: Failed to load texture " << path << std::endl;
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

int Texture::getWidth() const {
    return m_width;
}

int Texture::getHeight() const {
    return m_height;
}
