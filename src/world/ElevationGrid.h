#pragma once

#include "pch.h"

#include "renderer/Renderer.h"

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
    bool setElevation(glm::ivec2 gridPos, Elevation elevation);
    void setElevationInCircle(glm::vec2 pos, float radius, Elevation elevation);
    void generateWaterMesh();
    bool canElevate(glm::ivec2 gridPos, Elevation elevation);
    float getElevationAtPos(glm::vec2 pos);
    Elevation getElevationAtGridPos(glm::ivec2 gridPos);
    SlopeVariant getTileSlopeVariant(glm::ivec2 tile);
    float getTileBaseElevation(glm::ivec2 tile);
    std::vector<glm::ivec2> getSurroundingTiles(glm::ivec2 gridPos);
    std::vector<glm::ivec2> getAdjacentGridPositions(glm::ivec2 gridPos);
    std::vector<glm::ivec2> getAdjacentGridPositions(glm::ivec2 gridPos, bool diagonals);
    bool isPositionWater(glm::vec2 pos);
    bool isTileWater(glm::ivec2 pos);
    bool isPositionSloped(glm::vec2 pos);
    bool isPositionSlopeCorner(glm::vec2 pos);
    bool isPositionInGrid(glm::vec2 pos) const;

    json save();
    void load(json saveData);
private:
    unsigned int m_rows;
    unsigned int m_cols;
    std::vector<std::vector<Elevation>> m_grid;

    unsigned int m_numIndices;
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<IndexBuffer> m_ib;
    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;

    std::vector<glm::vec2> getTileWaterVertices(glm::ivec2 gridPos);
};
