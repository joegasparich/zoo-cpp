#pragma once

#include "common.h"
#include "Area.h"
#include "ISerialisable.h"

class AreaManager : public ISerialisable {
public:
    // Lifecycle
    void setup();
    void cleanup();

    void formAreas(Wall& placedWall);
    void joinAreas(Wall& removedWall);
    void resetAreas();
    std::list<Area*> BFS(Area* start, Area* end) const;

    std::vector<Area*> getAreas() const;
    Area* getAreaById(std::string id) const;
    Area* getAreaAtPosition(Vector2 pos) const;

    void renderDebugAreaGrid();

    void serialise() override;

private:
    std::vector<Cell> floodFill(Cell startTile);

    std::map<std::string, std::unique_ptr<Area>> areas;
    std::unordered_map<std::string, Area*> tileAreaMap;

    bool isSetup = false;
};
