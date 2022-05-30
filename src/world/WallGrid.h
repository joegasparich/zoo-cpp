#pragma once

#include <renderer/ArrayTexture.h>
#include "pch.h"
#include "common.h"
#include "Registry.h"

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
    WallData data;
    Orientation orientation;
    glm::vec2 worldPos;
    glm::vec2 gridPos;
    bool exists;
    bool indestructable;
    bool isDoor;
};

struct GridPos {
    int x;
    int y;
    Orientation orientation;
};
struct SpriteInfo {
    unsigned int index;
    float elevation;
};

class WallGrid {
public:
    WallGrid(unsigned int cols, unsigned int rows);

    void setup();
    void cleanup();
    void render();
    void regenerateMesh();
    Wall* placeWallAtTile(WallData wall, glm::ivec2 tilePos, Side side);
    void deleteWall(Wall wall);
    void deleteWallAtTile(glm::ivec2 tilePos, Side side);
    bool isWallPosInMap(glm::ivec2 tilePos, Side side) const;
    Wall* getWallAtTile(glm::ivec2 tilePos, Side side);
    std::array<glm::ivec2, 2> getAdjacentTiles(const Wall& wall);
    std::array<glm::vec2, 2> getWallVertices(const Wall& wall);
    std::vector<Wall*> getSurroundingWalls(glm::ivec2 gridPos);

    static glm::vec2 wallToWorldPosition(glm::ivec2 gridPos, Orientation orientation);
    static GridPos getGridPosition(glm::ivec2 tilePos, Side side);
    static SpriteInfo getSpriteInfo(Wall& wall);

    json save();
    void load(json saveData);
private:
    std::vector<std::vector<Wall>> m_wallGrid;
    unsigned int m_cols;
    unsigned int m_rows;
    bool m_isSetup;

    std::unique_ptr<ArrayTexture> m_textureArray;
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<IndexBuffer> m_ib;
    unsigned int m_numIndices;
};
