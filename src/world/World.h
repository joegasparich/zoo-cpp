#pragma once

#include <entities/Entity.h>
#include "pch.h"

#include "BiomeGrid.h"
#include "ElevationGrid.h"
#include "WallGrid.h"

class World {
public:
    World(unsigned int width, unsigned int height);

    // Lifecycle
    void setup();
    void cleanup();
    void update();
    void postUpdate();
    void render();

    // Functionality
    void registerTileObject(Entity* tileObject);
    void unregisterTileObject(Entity* tileObject);

    // Getters & Setters
    bool isPositionInMap(glm::vec2 pos) const;
    static Side getQuadrantAtPos(glm::vec2 pos);
    Entity* getTileObjectAtPosition(glm::vec2 pos);

    // Save & Load
    json save();
    void load(json saveData);

    std::unique_ptr<BiomeGrid> m_biomeGrid;
    std::unique_ptr<ElevationGrid> m_elevationGrid;
    std::unique_ptr<WallGrid> m_wallGrid;
private:
    unsigned int m_width;
    unsigned int m_height;

    std::unordered_map<unsigned int, Entity*> m_tileObjects;
    std::unordered_map<std::string, Entity*> m_tileObjectMap;
};
