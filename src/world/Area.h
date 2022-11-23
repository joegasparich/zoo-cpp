#pragma once

#include "common.h"
#include "WallGrid.h"

class Area {
public:
    Area(std::string id);
    virtual ~Area();

    void addAreaConnection(Area* area, Wall* door);
    void removeAreaConnection(Area* area, Wall* door);

    std::string id;
    Color colour;
    std::vector<Cell> tiles;
    std::unordered_map<Area*, std::unordered_set<Wall*>> connectedAreas;
};
