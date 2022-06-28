#pragma once

#include "pch.h"
#include "Registry.h"
#include <gfx/Renderer.h>

#define MAX_PATHS 32

struct Path {
    PathData* data;
    glm::vec2 pos;
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
    void regenerateMesh();

    Path* placePathAtTile(PathData* path, glm::ivec2 tilePos);
    Path* getPathAtTile(glm::ivec2 tilePos);

    static PathSpriteInfo getSpriteInfo(Path& path);

    json save();
    void load(json saveData);
private:
    std::vector<std::vector<Path>> m_grid;
    unsigned int m_rows;
    unsigned int m_cols;
    bool m_isSetup = false;

    // Rendering
    std::unique_ptr<ArrayTexture> m_textureArray;
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<IndexBuffer> m_ib;
    unsigned int m_numIndices;

    std::string m_elevationListenerHandle;
};
