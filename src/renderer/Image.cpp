#include "stb_image/stb_image.h"

#include "Image.h"

Image::Image(const std::string &filePath) : m_filePath(filePath) {
    stbi_set_flip_vertically_on_load(0);
    m_buffer = stbi_load(filePath.c_str(), &m_width, &m_height, &m_bpp, 4);
}

Image::~Image() {
    stbi_image_free(m_buffer);
}
