#pragma once

#include "common.h"
#include "Registry.h"
#include "Renderer.h"
#include "ISerialisable.h"

#define MAX_PATHS 32

struct Path {
    PathData* data;
    Cell pos;
    bool exists;
    bool indestructable;
    Color overrideColour = WHITE; // Gets reset after each frame
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

class PathGrid : public ISerialisable {
public:
    PathGrid(unsigned int mRows, unsigned int mCols);

    void setup();
    void cleanup();
    void render();

    void serialise() override;

    Path* placePathAtTile(PathData* path, Cell tilePos);
    void removePathAtTile(Cell tilePos);
    Path* getPathAtTile(Cell tilePos);

    static PathSpriteInfo getSpriteInfo(Path& path);

private:
    std::vector<std::vector<Path>> grid;
    unsigned int rows;
    unsigned int cols;
    bool isSetup = false;
};
