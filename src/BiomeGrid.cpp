#include "BiomeGrid.h"
#include "util/math.h"
#include "constants/world.h"
#include "Game.h"

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

BiomeChunk::BiomeChunk(unsigned int x, unsigned int y, unsigned int cols, unsigned int rows) :
        m_x{x},
        m_y{y},
        m_cols{cols},
        m_rows{rows} {}

void BiomeChunk::setup() {
    for (int i = 0; i < m_cols; i++) {
        m_grid.emplace_back();
        for (int j = 0; j < m_rows; j++) {
            m_grid.at(i).push_back(BiomeSquare{{Biome::Grass, Biome::Grass, Biome::Grass, Biome::Grass}});
        }
    }

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
    std::vector<std::array<float, 5>> vertices{};
    std::vector<std::array<unsigned int, 3>> indices{};

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
                    auto elevation = Game::get().m_stage->m_world->m_elevationGrid->getElevationAtPos((vertex + glm::vec2{m_x, m_y}) / 2.0f);
                    vertices.push_back({vertex.x, vertex.y - (elevation * 2.0f), color.x, color.y, color.z});
                }
            }
        }
    }

    for (int i=0; i<m_cols; ++i) {
        for (int j=0; j<m_rows; ++j) {
            unsigned int base = (i * m_rows + j) * 12;
            indices.push_back({base + 0, base+ 1, base + 2});
            indices.push_back({base + 3, base+ 4, base + 5});
            indices.push_back({base + 6, base+ 7, base + 8});
            indices.push_back({base + 9, base+ 10, base + 11});
        }
    }

    m_va = std::make_unique<VertexArray>();
    m_vb = std::make_unique<VertexBuffer>(&vertices[0], sizeof(float) * vertices.size() * 5);

    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->push<float>(2);
    m_layout->push<float>(3);
    m_va->addBuffer(*m_vb, *m_layout);
    m_ib = std::make_unique<IndexBuffer>(&indices[0], indices.size() * 3);

    m_shader = std::make_unique<Shader>("./shaders/BiomeVertex.shader", "./shaders/BiomeFragment.shader");
};

void BiomeChunk::render() {
    m_va->bind();
    m_ib->bind();
    m_shader->bind();

    glm::mat4 proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view,  glm::vec3(0.0f, WINDOW_HEIGHT, 0.0f));
    auto cameraScreenPos = Renderer::get().m_camera.pos * CELL_SIZE * BIOME_SCALE;
    view = glm::scale(view,  glm::vec3(Renderer::get().m_camera.scale, Renderer::get().m_camera.scale, 1.0f));
    view = glm::translate(view,  glm::vec3(-cameraScreenPos.x, cameraScreenPos.y, 0.0f));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_x * CELL_SIZE, -(int)m_y * CELL_SIZE, 0));
    model = glm::scale(model, glm::vec3(CELL_SIZE, -CELL_SIZE, 0));

    glm::mat4 mvp = proj * view * model;
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

// -- BiomeGrid -- //

BiomeGrid::BiomeGrid(unsigned int cols, unsigned int rows) :
    m_cols{cols},
    m_rows{rows} {
    setup();
}

void BiomeGrid::setup() {
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

            m_chunkGrid.at(i).at(j).setup();
        }
    }
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

bool BiomeGrid::isChunkInGrid(unsigned int col, unsigned int row) const {
    return col < m_cols / CHUNK_SIZE && row < m_rows / CHUNK_SIZE;
}

float BiomeGrid::getQuadrantSlopeColour(glm::vec2 pos, Side quadrant) {
    auto slopeVariant = Game::get().m_stage->m_world->m_elevationGrid->getTileSlopeVariant(glm::floor(pos/ 2.0f));

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
