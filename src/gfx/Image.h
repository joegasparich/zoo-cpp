#pragma once

#include <string>

class Image {
public:
    Image(const std::string &filePath);

    virtual ~Image();

    unsigned char* m_buffer;
    int m_width, m_height, m_bpp;
    std::string m_filePath;
};
