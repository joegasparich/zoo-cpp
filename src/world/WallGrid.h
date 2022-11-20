#pragma once

#include "common.h"
#include "Registry.h"
#include "Renderer.h"

#define MAX_WALLS 32

enum class Orientation {
    Vertical = 0,
    Horizontal = 1
};

enum class WallSpriteIndex {
    Horizontal = 0,
    Vertical = 1,
    DoorHorizontal = 2,
    DoorVertical = 3,
    HillEast = 4,
    HillWest = 5,
    HillNorth = 6,
    HillSouth = 7,
};

struct Wall {
    WallData* data; // TODO: Switch to pointer to avoid duplicating heaps of data
    Orientation orientation;
    Vector2 worldPos;
    Cell gridPos;
    bool exists;
    bool indestructable;
    bool isDoor;
};

struct GridPos {
    int x;
    int y;
    Orientation orientation;
};
struct WallSpriteInfo {
    unsigned int index;
    float elevation;
};

class WallGrid {
public:
    WallGrid(unsigned int cols, unsigned int rows);

    void setup();
    void cleanup();
    void render();

    Wall* placeWallAtTile(WallData* wall, Cell tilePos, Side side);
    void deleteWall(Wall wall);
    void deleteWallAtTile(Cell tilePos, Side side);
    void placeDoor(Wall* wall);
    void removeDoor(Wall* wall);

    void updatePathfindingAtWall(const Wall& wall);

    bool isWallPosInMap(Cell tilePos, Side side) const;
    bool isWallGridPosInMap(Cell gridPos);
    Wall* getWallAtTile(Cell tilePos, Side side);
    Wall* getWallByGridPos(Cell gridPos);
    std::vector<Wall*> getAdjacentWalls(const Wall& wall);
    std::vector<Cell> getAdjacentTiles(const Wall& wall);
    std::array<Cell, 2> getWallVertices(const Wall& wall);
    std::vector<Wall*> getSurroundingWalls(Cell gridPos);
    bool isWallSloped(const Wall& wall);

    static Vector2 wallToWorldPosition(Cell gridPos, Orientation orientation);
    static GridPos getGridPosition(Cell tilePos, Side side);
    static WallSpriteInfo getSpriteInfo(Wall& wall, bool isDoor = false);

    json save();
    void load(json saveData);
private:
    bool shouldCheckForLoop(const Wall& wall);
    bool checkForLoop(Wall* startWall, Wall* currentWall = nullptr, std::unordered_set<std::string> checkedWalls = {}, unsigned int depth = 0);

    std::vector<std::vector<Wall>> grid;
    unsigned int cols;
    unsigned int rows;
    bool isSetup;
};
