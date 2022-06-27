#pragma once

#include "pch.h"

#include "gfx/Renderer.h"

#define CHUNK_SIZE 5
#define NUM_BIOMES 3
#define BIOME_SCALE 2.0f

enum class Biome {
    Grass,
    Sand,
    Snow
};

struct BiomeInfo {
    std::string name;
    glm::vec3 color;
};

inline BiomeInfo getBiomeInfo(Biome biome) {
    switch (biome) {
        case Biome::Grass: return {"Grass", glm::vec3{0.714, 0.835, 0.235}};
        case Biome::Sand: return {"Sand", glm::vec3{0.957, 0.8, 0.631}};
        case Biome::Snow: return {"Snow", glm::vec3{0.875, 0.965, 0.965}};
        default: return {};
    }
}

struct BiomeSquare {
    BiomeSquare(std::array<Biome, 4> q) : quadrants{q} {};
    std::array<Biome, 4> quadrants;

    void setQuadrant(Side side, Biome biome) {
        quadrants[(int)side] = biome;
    }
};

class BiomeChunk {
public:
    BiomeChunk(unsigned int x, unsigned int y, unsigned int cols, unsigned int rows);
    BiomeChunk(std::vector<std::vector<std::vector<Biome>>> gridData, unsigned int x, unsigned int y);

    void setup(VertexBufferLayout& layout, Shader& shader);

    void postUpdate();
    void generateMesh();
    void render();
    void setBiomeInRadius(glm::vec2 pos, float radius, Biome biome);
    bool isPositionInChunk(glm::vec2 pos);

    std::vector<std::vector<std::vector<Biome>>> save();

    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_rows;
    unsigned int m_cols;
    bool m_shouldRegenerate;

    std::vector<std::vector<BiomeSquare>> m_grid;

    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<IndexBuffer> m_ib;
    VertexBufferLayout* m_layout;
    Shader* m_shader;
};

class BiomeGrid {
public:
    BiomeGrid(unsigned int cols, unsigned int rows);

    void setup();
    void cleanup();
    void postUpdate();
    void render();
    void setBiomeInRadius(glm::vec2 pos, float radius, Biome biome);
    void redrawChunksInRadius(glm::vec2 pos, float radius);
    void redrawAllChunks();
    bool isChunkInGrid(int col, int row) const;

    json save();
    void load(json saveData);

    static float getQuadrantSlopeColour(glm::vec2 pos, Side quadrant);
private:
    std::vector<BiomeChunk*> getChunksInRadius(glm::vec2 pos, float radius);

    unsigned int m_rows;
    unsigned int m_cols;
    bool m_isSetup = false;

    std::unique_ptr<VertexBufferLayout> m_layout;
    std::unique_ptr<Shader> m_shader;

    std::vector<std::vector<BiomeChunk>> m_chunkGrid;

    std::string m_elevationListenerHandle;
};
