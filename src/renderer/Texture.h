#pragma once

#include "pch.h"

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const;
    int getHeight() const;
private:
    unsigned int m_rendererID;
    std::string m_filePath;
    unsigned char* m_localBuffer;
    int m_width, m_height, m_bpp;
};
