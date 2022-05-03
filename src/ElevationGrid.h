#pragma once

#include <vector>
#include "glm/glm.hpp"

#define ELEVATION_HEIGHT 0.5f

enum class Elevation {
    Water = -1,
    Flat = 0,
    Hill = 1
};

enum class SlopeVariant {
    Flat,
    S, E, W, N,
    NW, NE, SW, SE,
    INW, INE, ISW, ISE,
    I1, I2
};

class ElevationGrid {
public:
    ElevationGrid(unsigned int rows, unsigned int cols);

    void setup();
    void reset();
    void setElevation(glm::ivec2 gridPos, Elevation elevation);
    void setElevationInCircle(glm::vec2 pos, float radius, Elevation elevation);
    bool canElevate(glm::ivec2 gridPos, Elevation elevation);
    float getElevationAtPos(glm::vec2 pos);
    Elevation getElevationAtGridPos(glm::ivec2 gridPos);
    SlopeVariant getTileSlopeVariant(glm::ivec2 tile);
    float getTileBaseElevation(glm::ivec2 tile);
    std::vector<glm::vec2> getSurroundingTiles(glm::ivec2 gridPos);
    std::vector<glm::vec2> getAdjacentGridPositions(glm::ivec2 gridPos);
    std::vector<glm::vec2> getAdjacentGridPositions(glm::ivec2 gridPos, bool diagonals);
    bool isPositionSloped(glm::vec2 pos);
    bool isPositionSlopeCorner(glm::vec2 pos);
    bool isPositionInGrid(glm::vec2 pos) const;
    void renderDebug();

private:
    unsigned int m_rows;
    unsigned int m_cols;
    std::vector<std::vector<Elevation>> m_grid;
};
