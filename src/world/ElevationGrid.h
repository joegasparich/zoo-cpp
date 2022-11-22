#pragma once

#include "common.h"

#include "Renderer.h"

#define ELEVATION_HEIGHT 0.5f

enum class Elevation : int {
    Water = -1,
    Flat = 0,
    Hill = 1
};

enum class SlopeVariant {
    Flat,
    S, E, W, N,
    NW, NE, SW, SE,
    INW, INE, ISW, ISE,
    I1, I2
};

class ElevationGrid {
public:
    ElevationGrid(unsigned int rows, unsigned int cols);

    void setup();
    void cleanup();
    void render();
    void renderDebug();

    bool setElevation(Cell gridPos, Elevation elevation);
    void setElevationInCircle(Vector2 pos, float radius, Elevation elevation);
    void generateWaterMesh();
    bool canElevate(Cell gridPos, Elevation elevation);

    float getElevationAtPos(Vector2 pos);
    Elevation getElevationAtGridPos(Cell gridPos);
    SlopeVariant getTileSlopeVariant(Cell tile);
    float getTileBaseElevation(Cell tile);
    std::vector<Cell> getSurroundingTiles(Cell gridPos);
    std::vector<Cell> getAdjacentGridPositions(Cell gridPos);
    std::vector<Cell> getAdjacentGridPositions(Cell gridPos, bool diagonals);
    bool isPositionWater(Vector2 pos);
    bool isTileWater(Cell pos);
    bool isPositionSloped(Vector2 pos);
    bool isPositionSlopeCorner(Vector2 pos);
    bool isPositionInGrid(Vector2 pos) const;

    json save();
    void load(json saveData);
private:
    unsigned int rows;
    unsigned int cols;
    std::vector<std::vector<Elevation>> grid;
    std::vector<std::vector<Vector2>> polygons;

    std::vector<Vector2> getTileWaterVertices(Cell gridPos);
};
