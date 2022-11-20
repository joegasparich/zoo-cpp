#pragma once

#include "common.h"

struct Tile {
    bool connections[8] = { true, true, true, true, true, true, true, true };
    bool accessible = true;
};

struct Node {
    Cell parent = { -1, -1 };
    double gCost = -1.0;
    double hCost = -1.0;
    double fCost = -1.0;
};

// Creating a shortcut for tuple<int, int, int> type
typedef std::tuple<double, int, int> Tuple;

class Pathfinder {
public:
    Pathfinder(unsigned int width, unsigned int height);

    std::vector<Cell> getPath(Cell from, Cell to);
    void setAccessibility(Cell tile, Direction direction, bool accessible);
    bool isAccessible(Cell tile);

    void drawDebugGrid();
private:
    std::vector<Cell> reconstructPath(const std::vector<std::vector<Node>>& cellDetails, const Cell& dest);
    std::vector<Cell> getNeighbours(Cell tile);
    double calculateHValue(Cell a, Cell b);
    bool isTileInGrid(Cell tile);

    std::unique_ptr<std::vector<std::vector<Tile>>> tileGrid;

    unsigned int cols;
    unsigned int rows;
    bool isSetup = false;
};
