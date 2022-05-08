#pragma once

#include "pch.h"
#include "Image.h"

class SpriteSheet {
public:
    int m_cellWidth;
    int m_cellHeight;
    Image* m_image;

    std::array<glm::vec2, 4> getTexCoords(unsigned int cellIndex);
    std::array<glm::vec2, 4> getTexCoords(unsigned int col, unsigned int row);
};
