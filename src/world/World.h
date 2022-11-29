#pragma once

#include "common.h"

#include "entities/Entity.h"
#include "BiomeGrid.h"
#include "ElevationGrid.h"
#include "WallGrid.h"
#include "PathGrid.h"
#include "AreaManager.h"
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

    void serialise() override;

    // Getters & Setters
    bool isPositionInMap(Vector2 pos) const;
    Entity* getTileObjectAtPosition(Vector2 pos);
    std::vector<Cell> getAccessibleAdjacentTiles(Cell tile);
    int getTileWalkability(Cell pos);

    static Side getQuadrantAtPos(Vector2 pos);
    static Cell getTileInDirection(Cell tile, Side direction);
    unsigned int getWidth () const;
    unsigned int getHeight () const;

    std::unique_ptr<BiomeGrid> biomeGrid;
    std::unique_ptr<ElevationGrid> elevationGrid;
    std::unique_ptr<WallGrid> wallGrid;
    std::unique_ptr<PathGrid> pathGrid;
    std::unique_ptr<AreaManager> areaManager;
    std::unique_ptr<Pathfinder> pathfinder;
private:
    void renderDebugCellGrid();

    unsigned int width;
    unsigned int height;

    std::unordered_map<unsigned int, Entity*> tileObjects;
    std::unordered_map<std::string, Entity*> tileObjectsMap;

    bool isSetup = false;
};
