#pragma once

#include "common.h"

#include "entities/Entity.h"
#include "BiomeGrid.h"
#include "ElevationGrid.h"
#include "WallGrid.h"
#include "Area.h"
#include "PathGrid.h"
#include "Pathfinder.h"
#include "ISerialisable.h"

class World : public ISerialisable {
public:
    World(unsigned int width, unsigned int height);
    virtual ~World();

    // Lifecycle
    void setup();
    void cleanup();
    void preUpdate();
    void update();
    void postUpdate();
    void render();
    void renderDebug();

    // Functionality
    void registerTileObject(Entity* tileObject);
    void unregisterTileObject(Entity* tileObject);
    void formAreas(Wall& placedWall);
    void joinAreas(Wall& removedWall);
    void resetAreas();

    void serialise() override;

    // Getters & Setters
    bool isPositionInMap(Vector2 pos) const;
    Cell getTileInDirection(Cell tile, Side direction);
    static Side getQuadrantAtPos(Vector2 pos);
    Entity* getTileObjectAtPosition(Vector2 pos);
    std::vector<Cell> getAccessibleAdjacentTiles(Cell tile);
    std::vector<Area*> getAreas();
    Area* getAreaById(std::string id);
    Area* getAreaAtPosition(Vector2 pos);
    int getTileWalkability(Cell pos);

    std::unique_ptr<BiomeGrid> biomeGrid;
    std::unique_ptr<ElevationGrid> elevationGrid;
    std::unique_ptr<WallGrid> wallGrid;
    std::unique_ptr<PathGrid> pathGrid;
    std::unique_ptr<Pathfinder> pathfinder;
    std::map<std::string, std::unique_ptr<Area>> areas;
private:
    std::vector<Cell> floodFill(Cell startTile);
    void renderDebugCellGrid();
    void renderDebugAreaGrid();

    unsigned int width;
    unsigned int height;

    std::unordered_map<unsigned int, Entity*> tileObjects;
    std::unordered_map<std::string, Entity*> tileObjectsMap;
    std::unordered_map<std::string, Area*> tileAreaMap;

    bool isSetup = false;
};
