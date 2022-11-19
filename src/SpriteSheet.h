#pragma once

#include "common.h"

class SpriteSheet {
public:
    int cellWidth;
    int cellHeight;
    std::string texturePath;
    Texture* texture;

    Rectangle getCellBounds(unsigned int cellIndex);
    Rectangle getCellBounds(unsigned int col, unsigned int row);
};
