#include <GL/glew.h>
#include "stb_image/stb_image.h"

#include "ArrayTexture.h"
#include "Renderer.h"

ArrayTexture::ArrayTexture(int width, int height, int maxLayers) : m_width(width), m_height(height), m_maxLayers(maxLayers) {
    GL_CALL(glGenTextures(1, &m_rendererID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID));

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    GL_CALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, maxLayers));
}
ArrayTexture::~ArrayTexture() {
    GL_CALL(glDeleteTextures(1, &m_rendererID));
};

void ArrayTexture::pushTexture(const Image& image) {
    if (image.m_width > m_width || image.m_height > m_height) {
        std::cout << "Warning: Texture does not fit in array texture bounds" << std::endl;
    }

    std::cout << "Pushing texture " << image.m_filePath << " onto layer " << m_layers << std::endl;
    GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_layers, image.m_width, image.m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.m_buffer));

    m_layers++;
}

void ArrayTexture::bind(unsigned int slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID));
}

void ArrayTexture::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}

int ArrayTexture::getWidth() const {
    return m_width;
}

int ArrayTexture::getHeight() const {
    return m_height;
}
