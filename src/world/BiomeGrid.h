#pragma once

#include "common.h"

#include "Renderer.h"
#include "ISerialisable.h"

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
    Color color;
};

inline BiomeInfo getBiomeInfo(Biome biome) {
    switch (biome) {
        case Biome::Grass: return {"Grass", {182, 213, 60, 255}};
        case Biome::Sand: return {"Sand", {244, 204, 161, 255}};
        case Biome::Snow: return {"Snow", {223, 246, 246, 255}};
        default: return {};
    }
}

struct BiomeTriangle {
    Color color;
    std::array<Vector2, 3> vertices;
};

struct BiomeCell {
    BiomeCell(std::array<Biome, 4> q) : quadrants{q} {};
    std::array<Biome, 4> quadrants;

    void setQuadrant(Side side, Biome biome) {
        quadrants[int(side)] = biome;
    }
};

class BiomeChunk {
public:
    BiomeChunk(unsigned int x, unsigned int y, unsigned int cols, unsigned int rows);
    BiomeChunk(std::vector<std::vector<std::vector<Biome>>> gridData, unsigned int x, unsigned int y);

    void setup();

    void postUpdate();
    void regenerateMesh();
    void render();
    void setBiomeInRadius(Vector2 pos, float radius, Biome biome);
    bool isPositionInChunk(Vector2 pos);

    std::vector<std::vector<std::vector<Biome>>> save();
    void load(std::vector<std::vector<std::vector<Biome>>> gridData);

    virtual ~BiomeChunk ();

    bool shouldRegenerate = false;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int rows = 0;
    unsigned int cols = 0;

private:
    bool isSetup = false;

    std::vector<std::vector<BiomeCell>> grid;
    Mesh chunkMesh = {0};
};

class BiomeGrid : public ISerialisable {
public:
    BiomeGrid(unsigned int cols, unsigned int rows);

    void setup();
    void cleanup();
    void postUpdate();
    void render();
    void renderChunkDebug();

    void serialise() override;

    void setBiomeInRadius(Vector2 pos, float radius, Biome biome);
    void regenerateChunksInRadius(Vector2 pos, float radius);
    void regenerateAllChunks();

    bool isChunkInGrid(int col, int row) const;
    BiomeChunk* getChunk(int col, int row) const;
    std::vector<BiomeChunk*> getChunksInRadius(Vector2 pos, float radius);
    static float getQuadrantSlopeColour(Vector2 pos, Side quadrant);

private:
    unsigned int rows;
    unsigned int cols;
    bool isSetup = false;

    std::vector<std::vector<std::unique_ptr<BiomeChunk>>> chunkGrid;

    std::string elevationListenerHandle;
};
