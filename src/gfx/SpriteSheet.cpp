#include "SpriteSheet.h"

std::array<glm::vec2, 4> SpriteSheet::getTexCoords(unsigned int cellIndex) {
    auto cols = m_image->m_width / m_cellWidth;

    return getTexCoords(cellIndex % cols, cellIndex / cols);
}

std::array<glm::vec2, 4> SpriteSheet::getTexCoords(unsigned int col, unsigned int row) {
    auto cols = m_image->m_width / m_cellWidth;
    auto rows = m_image->m_height / m_cellHeight;
    auto xFrac = (float)m_cellWidth / (float)m_image->m_width;
    auto yFrac = (float)m_cellHeight / (float)m_image->m_height;

    if (col >= cols || row >= rows) {
        std::cout << "Cell out of bounds " << col << ", " << row << std::endl;
        return {};
    }

    return {
        glm::vec2{
            (float)col * xFrac,
            (float)row * yFrac
        },
        glm::vec2{
            (float)(col + 1) * xFrac,
            (float)row * yFrac
        },
        glm::vec2{
            (float)(col + 1) * xFrac,
            (float)(row + 1) * yFrac
        },
        glm::vec2{
            (float)col * xFrac,
            (float)(row + 1) * yFrac
        }
    };
}
