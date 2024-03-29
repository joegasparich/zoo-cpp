#pragma once

#include <future>
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

// Creating a shortcut for tuple<double, int, int> type
typedef std::tuple<double, int, int> Tuple;

typedef std::vector<Cell> path;

class Pathfinder {
public:
    Pathfinder(unsigned int width, unsigned int height);

    path getPath(Cell from, Cell to);
    std::string requestAsyncPath(Cell from, Cell to);
    bool asyncPathExists(const std::string& handle);
    bool asyncPathReady(const std::string& handle);
    path getAsyncPath(const std::string& handle);
    void setAccessibility(Cell tile, bool accessible);
    void setAccessibility(Cell tile, Direction direction, bool accessible);
    bool isAccessible(Cell tile);

    void drawDebugGrid();
private:
    path reconstructPath(const std::vector<std::vector<Node>>& cellDetails, const Cell& dest);
    path getNeighbours(Cell tile);
    double calculateHValue(Cell a, Cell b);
    bool isTileInGrid(Cell tile);

    std::unique_ptr<std::vector<std::vector<Tile>>> tileGrid;
    std::unique_ptr<std::map<std::string, std::future<path>>> pathRequests;

    unsigned int cols;
    unsigned int rows;
    bool isSetup = false;
};
