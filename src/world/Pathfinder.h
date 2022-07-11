#pragma once

#include "pch.h"

enum class Direction {
    N, NE, E, SE, S, SW, W, NW
};

struct Tile {
    bool connections[8] = { true, true, true, true, true, true, true, true };
    bool accessible = true;
};

struct Node {
    glm::ivec2 parent = { -1, -1 };
    double gCost = -1.0;
    double hCost = -1.0;
    double fCost = -1.0;
};

// Creating a shortcut for tuple<int, int, int> type
typedef std::tuple<double, int, int> Tuple;

class Pathfinder {
public:
    Pathfinder(unsigned int width, unsigned int height);

    std::vector<glm::ivec2> getPath(glm::ivec2 from, glm::ivec2 to);
    void setAccessibility(glm::ivec2 tile, Direction direction, bool accessible);
    bool isAccessible(glm::ivec2 tile);
private:
    std::vector<glm::ivec2> reconstructPath(const std::vector<std::vector<Node>>& cellDetails, const glm::ivec2& dest);
    std::vector<glm::ivec2> getNeighbours(glm::ivec2 tile);
    double calculateHValue(glm::ivec2 a, glm::ivec2 b);

    std::unique_ptr<std::vector<std::vector<Tile>>> m_tileGrid;

    unsigned int m_cols;
    unsigned int m_rows;
    bool m_isSetup = false;
};
