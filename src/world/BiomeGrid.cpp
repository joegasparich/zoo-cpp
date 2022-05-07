#include <constants/depth.h>
#include "BiomeGrid.h"
#include "util/math.h"
#include "Game.h"
#include "Zoo.h"

#define SLOPE_COLOUR_STRENGTH 0.3f

std::vector<glm::vec2> getQuadrantVertices(glm::vec2 pos, Side quadrant) {
    switch (quadrant) {
        case Side::North:
            return {glm::vec2{pos.x, pos.y}, glm::vec2{pos.x + 1, pos.y}, glm::vec2{pos.x + 0.5, pos.y + 0.5}};
        case Side::West:
            return {glm::vec2{pos.x, pos.y}, glm::vec2{pos.x, pos.y + 1}, glm::vec2{pos.x + 0.5, pos.y + 0.5}};
        case Side::South:
            return {glm::vec2{pos.x, pos.y + 1}, glm::vec2{pos.x + 1, pos.y + 1}, glm::vec2{pos.x + 0.5, pos.y + 0.5}};
        case Side::East:
            return {glm::vec2{pos.x + 1, pos.y}, glm::vec2{pos.x + 1, pos.y + 1}, glm::vec2{pos.x + 0.5, pos.y + 0.5}};
    }
}

// -- BiomeChunk -- //

struct BiomeVertex {
    glm::vec2 pos;
    glm::vec3 colour;
};

BiomeChunk::BiomeChunk(unsigned int x, unsigned int y, unsigned int cols, unsigned int rows) :
        m_x{x},
        m_y{y},
        m_cols{cols},
        m_rows{rows} {
    for (int i = 0; i < m_cols; i++) {
        m_grid.emplace_back();
        for (int j = 0; j < m_rows; j++) {
            m_grid.at(i).push_back(BiomeSquare{{Biome::Grass, Biome::Grass, Biome::Grass, Biome::Grass}});
        }
    }
}

BiomeChunk::BiomeChunk(std::vector<std::vector<std::vector<Biome>>> gridData, unsigned int x, unsigned int y) {
    m_x = x;
    m_y = y;
    m_cols = gridData.size();
    m_rows = gridData[0].size();

    auto rowIndex = 0;
    for (auto& row : gridData) {
        m_grid.emplace_back();
        for (auto& square : row) {
            m_grid.at(rowIndex).push_back(BiomeSquare{{
                square[0],
                square[1],
                square[2],
                square[3]
            }});
        }
        rowIndex++;
    }
}

void BiomeChunk::setup(VertexArray& va, VertexBufferLayout& layout, Shader& shader) {
    m_va = &va;
    m_layout = &layout;
    m_shader = &shader;

    m_vb = std::make_unique<VertexBuffer>(nullptr, CHUNK_SIZE * CHUNK_SIZE * 4 * 3 * sizeof(BiomeVertex), true);

    std::vector<std::array<unsigned int, 3>> indices{};
    for (int i=0; i<m_cols; ++i) {
        for (int j=0; j<m_rows; ++j) {
            unsigned int base = (i * m_rows + j) * 12;
            indices.push_back({base + 0, base+ 1, base + 2});
            indices.push_back({base + 3, base+ 4, base + 5});
            indices.push_back({base + 6, base+ 7, base + 8});
            indices.push_back({base + 9, base+ 10, base + 11});
        }
    }

    m_ib = std::make_unique<IndexBuffer>(&indices[0], indices.size() * 3);

    generateMesh();
}

void BiomeChunk::postUpdate() {
    if (m_shouldRegenerate) {
        // TODO: Investigate drawing one chunk per frame or something
        generateMesh();
        m_shouldRegenerate = false;
    }
}

void BiomeChunk::generateMesh() {
    std::vector<BiomeVertex> vertices{};

    for (int i=0; i<m_cols; ++i) {
        for (int j=0; j<m_rows; ++j) {
            auto square = m_grid.at(i).at(j);
            for (int q=0; q<4; q++) {
                // Apply biome color
                glm::vec3 color = getBiomeInfo(square.quadrants[q]).color;
                color = rgb2hsv(color);
                auto slopeColor = BiomeGrid::getQuadrantSlopeColour({i + m_x, j + m_y}, (Side)q);
                color = hsv2rgb({color.x, color.y, glm::clamp(color.z + slopeColor * SLOPE_COLOUR_STRENGTH, 0.0f, 1.0f)});

                // push vertices
                for (auto vertex : getQuadrantVertices({i, j}, (Side)q)) {
                    // Apply elevation
                    auto elevation = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos((vertex + glm::vec2{m_x, m_y}) / 2.0f);
                    // push vertex
                    vertices.push_back({{vertex.x, vertex.y - (elevation * 2.0f)},
                                        {color.x, color.y, color.z}});
                }
            }
        }
    }

    m_vb->updateData(&vertices[0], vertices.size() * sizeof(float) * 5, 0);
};

void BiomeChunk::render() {
    if (!Renderer::isPositionOnScreen({m_x / BIOME_SCALE, m_y / BIOME_SCALE}, CHUNK_SIZE)) return;

    m_va->addBuffer(*m_vb, *m_layout);
    m_va->bind();
    m_shader->bind();
    m_ib->bind();

    auto mvp = Renderer::getMVPMatrix(glm::vec3((float)m_x / BIOME_SCALE, (float)m_y / BIOME_SCALE, 0), 0.0f, DEPTH::BIOME, glm::vec3(1.0f / BIOME_SCALE), true);
    m_shader->setUniformMat4f("u_MVP", mvp);

    GL_CALL(glDrawElements(GL_TRIANGLES, m_cols * m_rows * 12, GL_UNSIGNED_INT, nullptr));
}

void BiomeChunk::setBiomeInRadius(glm::vec2 pos, float radius, Biome biome) {
    bool changed = false;

    for (float i = pos.x - radius; i < pos.x + radius; i++) {
        for (float j = pos.y - radius; j < pos.y + radius; j++) {
            auto cellPos = glm::vec2{floor(i), floor(j)};
            if (!this->isPositionInChunk(cellPos)) continue;

            // Get triangles in circle
            for (char q = 0; q < 4; q++) {
                auto side = (Side)q;
                for (auto point : getQuadrantVertices(cellPos, side)) {
                    if (pointInCircle(pos, radius, point)) {
                        auto xFloor = floor(cellPos.x);
                        auto yFloor = floor(cellPos.y);
                        if (m_grid[xFloor][yFloor].quadrants[(int)side] != biome) {
                            m_grid[xFloor][yFloor].setQuadrant(side, biome);
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

bool BiomeChunk::isPositionInChunk(glm::vec2 pos) {
    return pos.x >= 0 && pos.x < m_cols && pos.y >= 0 && pos.y < m_rows;
}

std::vector<std::vector<std::vector<Biome>>> BiomeChunk::save() {
    std::vector<std::vector<std::vector<Biome>>> chunkData;
    std::transform(m_grid.begin(), m_grid.end(), std::back_inserter(chunkData), [](std::vector<BiomeSquare>& row) {
        std::vector<std::vector<Biome>> rowData;
        std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](BiomeSquare& square) {
            return std::vector<Biome>{square.quadrants.begin(), square.quadrants.end()};
        });
       return rowData;
    });
    return chunkData;
}

// -- BiomeGrid -- //

BiomeGrid::BiomeGrid(unsigned int cols, unsigned int rows) :
    m_cols{cols},
    m_rows{rows} {
    setup();
}

void BiomeGrid::setup() {
    m_va = std::make_unique<VertexArray>();
    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->push<float>(2);
    m_layout->push<float>(3);
    m_shader = std::make_unique<Shader>("./shaders/BiomeVertex.shader", "./shaders/BiomeFragment.shader");

    auto chunkCols = m_cols / CHUNK_SIZE + (m_cols % CHUNK_SIZE != 0);
    auto chunkRows = m_rows / CHUNK_SIZE + (m_rows % CHUNK_SIZE != 0);

    for (unsigned int i = 0; i < chunkCols; i++) {
        m_chunkGrid.emplace_back();
        for (unsigned int j = 0; j < chunkRows; j++) {
            m_chunkGrid.at(i).push_back(BiomeChunk{
                i * CHUNK_SIZE,
                j * CHUNK_SIZE,
                i == chunkCols - 1 && m_cols % CHUNK_SIZE != 0 ? m_cols & CHUNK_SIZE : CHUNK_SIZE,
                j == chunkRows - 1 && m_rows % CHUNK_SIZE != 0 ? m_rows & CHUNK_SIZE : CHUNK_SIZE,
            });

            m_chunkGrid.at(i).at(j).setup(*m_va, *m_layout, *m_shader);
        }
    }
}

void BiomeGrid::reset() {
    m_cols = 0;
    m_rows = 0;
    m_chunkGrid.clear();
}

void BiomeGrid::postUpdate() {
    for (auto & i : m_chunkGrid) {
        for (auto & j : i) {
            j.postUpdate();
        }
    }
}

void BiomeGrid::render() {
    for (auto & i : m_chunkGrid) {
        for (auto & j : i) {
            j.render();
        }
    }
}

void BiomeGrid::setBiomeInRadius(glm::vec2 pos, float radius, Biome biome) {
    pos *= BIOME_SCALE; radius *= BIOME_SCALE;

    for (auto chunk : getChunksInRadius(pos, radius)) {
        chunk->setBiomeInRadius(pos - glm::vec2{chunk->m_x, chunk->m_y}, radius, biome);
    }
}

void BiomeGrid::redrawChunksInRadius(glm::vec2 pos, float radius) {
    for (auto chunk : getChunksInRadius(pos, radius)) {
        chunk->m_shouldRegenerate = true;
    }
}

void BiomeGrid::redrawAllChunks() {
    for (auto & i : m_chunkGrid) {
        for (auto & j : i) {
            j.m_shouldRegenerate = true;
        }
    }
}

std::vector<BiomeChunk*> BiomeGrid::getChunksInRadius(glm::vec2 pos, float radius) {
    unsigned int floorX = floor(pos.x / CHUNK_SIZE);
    unsigned int floorY = floor(pos.y / CHUNK_SIZE);

    std::vector<BiomeChunk*> chunks{};

    // TODO: Only handles 3x3 around the pos. Change this to calculate required area
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (isChunkInGrid(floorX + i, floorY + j)
                && circleIntersectsRect({(floorX + i) * CHUNK_SIZE, (floorY + j) * CHUNK_SIZE}, {CHUNK_SIZE, CHUNK_SIZE}, pos, radius)) {
                chunks.push_back(&m_chunkGrid.at(floorX + i).at(floorY + j));
            }
        }
    }

    return chunks;
}

bool BiomeGrid::isChunkInGrid(int col, int row) const {
    return col >= 0 && col < m_cols / CHUNK_SIZE && row >= 0 && row < m_rows / CHUNK_SIZE;
}

json BiomeGrid::save() {
    // 5D Chess
    std::vector<std::vector<std::vector<std::vector<std::vector<Biome>>>>> gridData;
    std::transform(m_chunkGrid.begin(), m_chunkGrid.end(), std::back_inserter(gridData), [](std::vector<BiomeChunk>& row) {
        std::vector<std::vector<std::vector<std::vector<Biome>>>> rowData;
        std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](BiomeChunk& chunk) {
            return chunk.save();
        });
        return rowData;
    });

    return json{
        {"grid", gridData},
        {"cols", m_cols},
        {"rows", m_rows}
    };

}

void BiomeGrid::load(json saveData) {
    reset();

    m_cols = saveData["cols"].get<unsigned int>();
    m_rows = saveData["rows"].get<unsigned int>();

    auto gridData = saveData["grid"].get<std::vector<std::vector<std::vector<std::vector<std::vector<Biome>>>>>>();

    unsigned int i = 0;
    for (auto& row : gridData) {
        unsigned int j = 0;
        m_chunkGrid.emplace_back();
        for (auto& chunkData : row) {
            m_chunkGrid.at(i).push_back(BiomeChunk{
                chunkData, i * CHUNK_SIZE, j * CHUNK_SIZE
            });
            m_chunkGrid.at(i).at(j).setup(*m_va, *m_layout, *m_shader);
            j++;
        }
        i++;
    }
}

float BiomeGrid::getQuadrantSlopeColour(glm::vec2 pos, Side quadrant) {
    auto slopeVariant = Zoo::zoo->m_world->m_elevationGrid->getTileSlopeVariant(glm::floor(pos / 2.0f));

    float F = 0.0f;
    float NW = 1.0f, N = 0.75f, W = 0.5f, NE = 0.25f;
    float SW = -0.25f, E = -0.5f, S = -0.75f, SE = -1.0f;

    auto xRel = (float)std::fmod(pos.x / 2.0f, 1);
    auto yRel = (float)std::fmod(pos.y / 2.0f, 1);

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
            return xRel - yRel < 0 || (xRel - yRel == 0 && (quadrant == Side::South || quadrant == Side::West))
               ? N
               : W;
        case SlopeVariant::INE:
            return xRel + yRel > 0.5 || (xRel + yRel == 0.5 && (quadrant == Side::South || quadrant == Side::East))
               ? N
               : E;
        case SlopeVariant::ISW:
            return xRel + yRel < 0.5 || (xRel + yRel == 0.5 && (quadrant == Side::North || quadrant == Side::West))
               ? S
               : W;
        case SlopeVariant::ISE:
            return yRel - xRel < 0 || (yRel - xRel == 0 && (quadrant == Side::North || quadrant == Side::East))
               ? S
               : E;
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
