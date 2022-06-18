#pragma once

#include <entities/Entity.h>
#include "pch.h"

#include "BiomeGrid.h"
#include "ElevationGrid.h"
#include "WallGrid.h"
#include "Area.h"

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
    void formAreas(Wall& placedWall);
    void joinAreas(Wall& removedWall);
    void resetAreas();

    // Getters & Setters
    bool isPositionInMap(glm::vec2 pos) const;
    glm::ivec2 getTileInDirection(glm::ivec2 tile, Side direction);
    static Side getQuadrantAtPos(glm::vec2 pos);
    Entity* getTileObjectAtPosition(glm::vec2 pos);
    std::vector<glm::ivec2> getAccessibleAdjacentTiles(glm::ivec2 tile);
    std::vector<Area*> getAreas();
    Area& getAreaById(std::string id);

    // Save & Load
    json save();
    void load(json saveData);

    std::unique_ptr<BiomeGrid> m_biomeGrid;
    std::unique_ptr<ElevationGrid> m_elevationGrid;
    std::unique_ptr<WallGrid> m_wallGrid;
    std::map<std::string, std::unique_ptr<Area>> m_areas;
private:
    std::vector<glm::ivec2> floodFill(glm::ivec2 startTile);

    unsigned int m_width;
    unsigned int m_height;

    std::unordered_map<unsigned int, Entity*> m_tileObjects;
    std::unordered_map<std::string, Entity*> m_tileObjectMap;
    std::unordered_map<std::string, Area*> m_tileAreaMap;
};
