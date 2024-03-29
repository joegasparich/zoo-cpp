#include "SpriteSheet.h"

Rectangle SpriteSheet::getCellBounds(unsigned int cellIndex) {
    auto cols = texture->width / cellWidth;

    return getCellBounds(cellIndex % cols, cellIndex / cols);
}

Rectangle SpriteSheet::getCellBounds(unsigned int col, unsigned int row) {
    auto cols = texture->width / cellWidth;
    auto rows = texture->height / cellHeight;
    auto xFrac = float(cellWidth) / float(texture->width);
    auto yFrac = float(cellHeight) / float(texture->height);

    if (col >= cols || row >= rows) {
        TraceLog(LOG_WARNING, "Spritesheet cell out of bounds %u, %u", col, row);
        return {};
    }

    return {
        float(col * xFrac),
        float(row * yFrac),
        float(xFrac),
        float(yFrac)
    };
}
