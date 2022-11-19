#pragma once

#include "common.h"
#include "Registry.h"
#include "Renderer.h"

#define MAX_PATHS 32

struct Path {
    PathData* data;
    Cell pos;
    bool exists;
    bool indestructable;
};

enum class PathSpriteIndex {
    Flat = 0,
    HillEast = 1,
    HillWest = 2,
    HillNorth = 3,
    HillSouth = 4,
};

struct PathSpriteInfo {
    unsigned int index;
    float elevation;
};

class PathGrid {
public:
    PathGrid(unsigned int mRows, unsigned int mCols);

    void setup();
    void cleanup();
    void render();

    Path* placePathAtTile(PathData* path, Cell tilePos);
    Path* getPathAtTile(Cell tilePos);

    static PathSpriteInfo getSpriteInfo(Path& path);

    json save();
    void load(json saveData);
private:
    std::vector<std::vector<Path>> grid;
    unsigned int rows;
    unsigned int cols;
    bool isSetup = false;
};
