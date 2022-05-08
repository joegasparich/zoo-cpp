#pragma once

#include "pch.h"

#include "BiomeGrid.h"
#include "ElevationGrid.h"
#include "WallGrid.h"

class World {
public:
    World(unsigned int width, unsigned int height);

    void setup();
    void reset();
    void update();
    void postUpdate();
    void render();
    bool isPositionInMap(glm::vec2 pos) const;

    static Side getQuadrantAtPos(glm::vec2 pos);

    json save();
    void load(json saveData);

    std::unique_ptr<BiomeGrid> m_biomeGrid;
    std::unique_ptr<ElevationGrid> m_elevationGrid;
    std::unique_ptr<WallGrid> m_wallGrid;
private:
    unsigned int m_width;
    unsigned int m_height;
};
