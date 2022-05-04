#pragma once

#include <memory>
#include "BiomeGrid.h"
#include "ElevationGrid.h"

class World {
public:
    World(unsigned int width, unsigned int height);

    void setup();
    void update();
    void postUpdate();
    void render();

    std::unique_ptr<BiomeGrid> m_biomeGrid;
    std::unique_ptr<ElevationGrid> m_elevationGrid;
private:
    unsigned int m_width;
    unsigned int m_height;
};
