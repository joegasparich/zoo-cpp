#include "BiomeGrid.h"
#include "util/jmath.h"
#include "ZooScene.h"
#include "Messenger.h"
#include "ElevationGrid.h"
#include "util/color.h"
#include "constants/world.h"

#define SLOPE_COLOUR_STRENGTH 0.3f

std::array<Vector2, 3> getQuadrantVertices(Vector2 pos, Side quadrant) {
    switch (quadrant) {
        case Side::North:
            return {Vector2{pos.x, pos.y}, Vector2{pos.x + 0.5f, pos.y + 0.5f}, Vector2{pos.x + 1, pos.y}};
        case Side::West:
            return {Vector2{pos.x, pos.y}, Vector2{pos.x, pos.y + 1}, Vector2{pos.x + 0.5f, pos.y + 0.5f}};
        case Side::South:
            return {Vector2{pos.x, pos.y + 1}, Vector2{pos.x + 1, pos.y + 1}, Vector2{pos.x + 0.5f, pos.y + 0.5f}};
        case Side::East:
            return {Vector2{pos.x + 1, pos.y}, Vector2{pos.x + 0.5f, pos.y + 0.5f}, Vector2{pos.x + 1, pos.y + 1}};
    }
}

// -- BiomeChunk -- //

BiomeChunk::BiomeChunk(unsigned int x, unsigned int y, unsigned int cols, unsigned int rows) :
    x{x},
    y{y},
    cols{cols},
    rows{rows},
    grid{},
    triangles{} {
    for (int i = 0; i < cols; i++) {
        grid.emplace_back();
        for (int j = 0; j < rows; j++) {
            grid.at(i).push_back(BiomeCell{{Biome::Grass, Biome::Grass, Biome::Grass, Biome::Grass}});
        }
    }
}

BiomeChunk::BiomeChunk(std::vector<std::vector<std::vector<Biome>>> gridData, unsigned int x, unsigned int y) : x{x}, y{y} {
    cols = gridData.size();
    rows = gridData[0].size();

    auto rowIndex = 0;
    for (auto& row : gridData) {
        grid.emplace_back();
        for (auto& square : row) {
            grid.at(rowIndex).push_back(BiomeCell{{
                square[0],
                square[1],
                square[2],
                square[3]
            }});
        }
        rowIndex++;
    }
}

void BiomeChunk::setup() {
    generateMesh();
}

void BiomeChunk::postUpdate() {
    if (shouldRegenerate) {
        // TODO: Investigate drawing one chunk per frame or something
        generateMesh();
        shouldRegenerate = false;
    }
}

void BiomeChunk::generateMesh() {
    // Maybe use GenMeshCustom?
    // - Can't use GenMeshCustom unless I change to 3D camera
    // Test performance on this

    triangles.clear();

    for (int i=0; i < cols; ++i) {
        for (int j=0; j < rows; ++j) {
            auto cell = grid.at(i).at(j);
            for (int q=0; q<4; q++) {
                // Apply biome color
                auto color = col2vec(getBiomeInfo(cell.quadrants[q]).color);
                color = rgb2hsv(color);
                auto slopeColor = BiomeGrid::getQuadrantSlopeColour({float(i + x), float(j + y)}, (Side)q);
                color = hsv2rgb({color.x, color.y, clamp(color.z + slopeColor * SLOPE_COLOUR_STRENGTH, 0.0f, 1.0f)});

                // generate triangle
                BiomeTriangle triangle{vec2Col(color)};
                int k = 0;
                for (auto vertex : getQuadrantVertices({float(i), float(j)}, (Side)q)) {
                    // Apply elevation
                    auto elevation = Root::zoo()->world->elevationGrid->getElevationAtPos((vertex + Vector2{float(x), float(y)}) / 2.0f);
                    // push vertex
                    triangle.vertices[k] = {vertex.x, vertex.y - (elevation * 2.0f)},
                    k++;
                }

                triangles.push_back(triangle);
            }
        }
    }
};

void BiomeChunk::render() {
    // Cull off-screen chunks
    if (!Root::renderer().isRectangleOnScreen({x / BIOME_SCALE, y / BIOME_SCALE, rows / BIOME_SCALE, cols / BIOME_SCALE})) return;

    for(auto triangle : triangles) {
        auto scale = WORLD_SCALE / BIOME_SCALE;
        auto [a,b,c] = triangle.vertices;
        auto pos = Vector2{float(x), float(y)} * scale;
        DrawTriangle(pos + a * scale, pos + b * scale, pos + c * scale, triangle.color);
    }
}

void BiomeChunk::setBiomeInRadius(Vector2 pos, float radius, Biome biome) {
    bool changed = false;

    for (float i = pos.x - radius; i < pos.x + radius; i++) {
        for (float j = pos.y - radius; j < pos.y + radius; j++) {
            auto cellPos = Vector2{floor(i), floor(j)};
            if (!this->isPositionInChunk(cellPos)) continue;

            // Get triangles in circle
            for (char q = 0; q < 4; q++) {
                auto side = (Side)q;
                for (auto point : getQuadrantVertices(cellPos, side)) {
                    if (pointInCircle(pos, radius, point)) {
                        auto xFloor = floor(cellPos.x);
                        auto yFloor = floor(cellPos.y);
                        if (grid[xFloor][yFloor].quadrants[int(side)] != biome) {
                            grid[xFloor][yFloor].setQuadrant(side, biome);
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    if (changed) {
        generateMesh();
    }
}

bool BiomeChunk::isPositionInChunk(Vector2 pos) {
    return pos.x >= 0 && pos.x < cols && pos.y >= 0 && pos.y < rows;
}

std::vector<std::vector<std::vector<Biome>>> BiomeChunk::save() {
    std::vector<std::vector<std::vector<Biome>>> chunkData;
    std::transform(grid.begin(), grid.end(), std::back_inserter(chunkData), [](std::vector<BiomeCell>& row) {
        std::vector<std::vector<Biome>> rowData;
        std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](BiomeCell& square) {
            return std::vector<Biome>{square.quadrants.begin(), square.quadrants.end()};
        });
       return rowData;
    });
    return chunkData;
}

// -- BiomeGrid -- //

BiomeGrid::BiomeGrid(unsigned int cols, unsigned int rows) : cols{cols}, rows{rows} {}

void BiomeGrid::setup() {
    TraceLog(LOG_TRACE, "Setting up biome grid");

    auto chunkCols = cols / CHUNK_SIZE + (cols % CHUNK_SIZE != 0);
    auto chunkRows = rows / CHUNK_SIZE + (rows % CHUNK_SIZE != 0);

    for (unsigned int i = 0; i < chunkCols; i++) {
        chunkGrid.emplace_back();
        for (unsigned int j = 0; j < chunkRows; j++) {
            chunkGrid.at(i).push_back(BiomeChunk{
                i * CHUNK_SIZE,
                j * CHUNK_SIZE,
                i == chunkCols - 1 && cols % CHUNK_SIZE != 0 ? cols & CHUNK_SIZE : CHUNK_SIZE,
                j == chunkRows - 1 && rows % CHUNK_SIZE != 0 ? rows & CHUNK_SIZE : CHUNK_SIZE,
            });

            chunkGrid.at(i).at(j).setup();
        }
    }

    elevationListenerHandle = Messenger::on(EventType::ElevationUpdated, [this](Event* _e) {
        auto e = static_cast<ElevationUpdatedEvent*>(_e);

        redrawChunksInRadius(e->pos * 2.0f, e->radius + 6.0f);
    });

    isSetup = true;
}

void BiomeGrid::cleanup() {
    cols = 0;
    rows = 0;
    chunkGrid.clear();

    Messenger::unsubscribe(EventType::ElevationUpdated, elevationListenerHandle);

    isSetup = false;
    elevationListenerHandle = "";
}

void BiomeGrid::postUpdate() {
    for (auto & i : chunkGrid) {
        for (auto & j : i) {
            j.postUpdate();
        }
    }
}

void BiomeGrid::render() {
    for (auto & i : chunkGrid) {
        for (auto & j : i) {
            j.render();
        }
    }
}

void BiomeGrid::setBiomeInRadius(Vector2 pos, float radius, Biome biome) {
    assert(isSetup);
    pos *= BIOME_SCALE; radius *= BIOME_SCALE;

    for (auto chunk : getChunksInRadius(pos, radius)) {
        chunk->setBiomeInRadius(pos - Vector2{float(chunk->x), float(chunk->y)}, radius, biome);
    }
}

void BiomeGrid::redrawChunksInRadius(Vector2 pos, float radius) {
    assert(isSetup);
    for (auto chunk : getChunksInRadius(pos, radius)) {
        chunk->shouldRegenerate = true;
    }
}

void BiomeGrid::redrawAllChunks() {
    assert(isSetup);
    for (auto & i : chunkGrid) {
        for (auto & j : i) {
            j.shouldRegenerate = true;
        }
    }
}

std::vector<BiomeChunk*> BiomeGrid::getChunksInRadius(Vector2 pos, float radius) {
    assert(isSetup);
    unsigned int floorX = floor(pos.x / CHUNK_SIZE);
    unsigned int floorY = floor(pos.y / CHUNK_SIZE);

    std::vector<BiomeChunk*> chunks{};

    // TODO: Only handles 3x3 around the pos. Change this to calculate required area
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (isChunkInGrid(floorX + i, floorY + j)
                && circleIntersectsRect({float((floorX + i) * CHUNK_SIZE), float((floorY + j) * CHUNK_SIZE)}, {CHUNK_SIZE, CHUNK_SIZE}, pos, radius)) {
                chunks.push_back(&chunkGrid.at(floorX + i).at(floorY + j));
            }
        }
    }

    return chunks;
}

bool BiomeGrid::isChunkInGrid(int col, int row) const {
    assert(isSetup);
    return col >= 0 && col < cols / CHUNK_SIZE && row >= 0 && row < rows / CHUNK_SIZE;
}

void BiomeGrid::serialise() {
    if (Root::saveManager().mode == SerialiseMode::Loading) {
        cleanup();

        elevationListenerHandle = Messenger::on(EventType::ElevationUpdated, [this](Event* _e) {
            auto e = static_cast<ElevationUpdatedEvent*>(_e);

            redrawChunksInRadius(e->pos * 2.0f, e->radius + 6.0f);
        });
    }

    Root::saveManager().SerialiseValue("cols", cols);
    Root::saveManager().SerialiseValue("rows", rows);

    std::function<json()> get = [&](){
        // 5D Chess
        std::vector<std::vector<std::vector<std::vector<std::vector<Biome>>>>> gridData;
        std::transform(chunkGrid.begin(), chunkGrid.end(), std::back_inserter(gridData), [](std::vector<BiomeChunk>& row) {
            std::vector<std::vector<std::vector<std::vector<Biome>>>> rowData;
            std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](BiomeChunk& chunk) {
                return chunk.save();
            });
            return rowData;
        });

        return gridData;
    };
    std::function<void(json)> set = [&](json data){
        auto gridData = data.get<std::vector<std::vector<std::vector<std::vector<std::vector<Biome>>>>>>();

        unsigned int i = 0;
        for (auto& row : gridData) {
            unsigned int j = 0;
            chunkGrid.emplace_back();
            for (auto& chunkData : row) {
                chunkGrid.at(i).push_back(BiomeChunk{
                    chunkData, i * CHUNK_SIZE, j * CHUNK_SIZE
                });
                chunkGrid.at(i).at(j).setup();
                j++;
            }
            i++;
        }
    };
    Root::saveManager().SerialiseValue("grid", get, set);

    if (Root::saveManager().mode == SerialiseMode::Loading)
        isSetup = true;
}

float BiomeGrid::getQuadrantSlopeColour(Vector2 pos, Side quadrant) {
    auto slopeVariant = Root::zoo()->world->elevationGrid->getTileSlopeVariant(flr(pos / 2.0f));

    float F = 0.0f;
    float NW = 1.0f, N = 0.75f, W = 0.5f, NE = 0.25f;
    float SW = -0.25f, E = -0.5f, S = -0.75f, SE = -1.0f;

    auto xRel = float(std::fmod(pos.x / 2.0f, 1));
    auto yRel = float(std::fmod(pos.y / 2.0f, 1));

    switch (slopeVariant) {
        case SlopeVariant::N:
            return N;
        case SlopeVariant::S:
            return S;
        case SlopeVariant::W:
            return W;
        case SlopeVariant::E:
            return E;
        case SlopeVariant::NW:
            return xRel + yRel > 0.5 || (xRel + yRel == 0.5 && (quadrant == Side::South || quadrant == Side::East))
               ? NW
               : F;
        case SlopeVariant::NE:
            return xRel - yRel < 0 || (xRel - yRel == 0 && (quadrant == Side::South || quadrant == Side::West))
               ? NE
               : F;
        case SlopeVariant::SW:
            return yRel - xRel < 0 || (yRel - xRel == 0 && (quadrant == Side::North || quadrant == Side::East))
               ? SW
               : F;
        case SlopeVariant::SE:
            return xRel + yRel < 0.5 || (xRel + yRel == 0.5 && (quadrant == Side::North || quadrant == Side::West))
               ? SE
               : F;
        case SlopeVariant::INW:
            if (xRel + yRel > 0.5f) return F;
            return (xRel + yRel == 0.5f && (quadrant == Side::South || quadrant == Side::East)) ? F : NW;
        case SlopeVariant::INE:
            if (yRel - xRel >= 0.5f) return F;
            return (yRel - xRel == 0.0f && (quadrant == Side::South || quadrant == Side::West)) ? F : NE;
        case SlopeVariant::ISW:
            if (xRel - yRel >= 0.5f) return F;
            return (xRel - yRel == 0.0f && (quadrant == Side::North || quadrant == Side::East)) ? F : SW;
        case SlopeVariant::ISE:
            if (xRel + yRel < 0.5f) return F;
            return (xRel + yRel == 0.5f && (quadrant == Side::North || quadrant == Side::West)) ? F : SE;
        case SlopeVariant::I1:
            return xRel + yRel > 0.5 || (xRel + yRel == 0.5 && (quadrant == Side::South || quadrant == Side::East))
               ? NW
               : SE;
        case SlopeVariant::I2:
            return xRel - yRel < 0 || (xRel - yRel == 0 && (quadrant == Side::South || quadrant == Side::West))
               ? NE
               : SW;
        case SlopeVariant::Flat:
        default:
            return F;
    }
}
