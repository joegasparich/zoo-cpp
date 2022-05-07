#include <constants/depth.h>
#include "ElevationGrid.h"
#include "constants/world.h"
#include "util/math.h"
#include "Debug.h"
#include "Zoo.h"

#define WATER_LEVEL 0.1f

ElevationGrid::ElevationGrid(unsigned int rows, unsigned int cols) : m_rows(rows), m_cols(cols) {}

void ElevationGrid::setup() {
    m_va = std::make_unique<VertexArray>();
    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->push<float>(2);
    m_shader = std::make_unique<Shader>("./shaders/WaterVertex.shader", "./shaders/WaterFragment.shader");

    for (unsigned int i = 0; i < m_cols; i++) {
        m_grid.emplace_back();
        for (unsigned int j = 0; j < m_rows; j++) {
            m_grid.at(i).push_back(Elevation::Flat);
        }
    }

    generateWaterMesh();
}

void ElevationGrid::reset() {
    m_cols = 0;
    m_rows = 0;
    m_grid.clear();

    m_numIndices = 0;
    m_vb = nullptr;
    m_ib = nullptr;
}

void ElevationGrid::render() {
    m_va->bind();
    m_ib->bind();
    m_shader->bind();

    auto mvp = Renderer::getMVPMatrix(glm::vec3(0.0f), 0.0f, DEPTH::WATER, glm::vec3(1.0f), true);
    m_shader->setUniformMat4f("u_MVP", mvp);

    GL_CALL(glDrawElements(GL_TRIANGLE_FAN, m_numIndices, GL_UNSIGNED_INT, nullptr));
}

void ElevationGrid::renderDebug() {
    if (m_grid.empty()) return;

    for (unsigned int i = 0; i < m_cols; i++) {
        for (unsigned int j = 0; j < m_rows; j++) {
            if (i < m_cols - 1) {
                Debug::drawLine(
                    {i * WORLD_SCALE, (j - (float)(int)m_grid.at(i).at(j) * ELEVATION_HEIGHT) * WORLD_SCALE},
                    {(i + 1) * WORLD_SCALE, (j - (float)(int)m_grid.at(i + 1).at(j) * ELEVATION_HEIGHT) * WORLD_SCALE},
                    {1.0f, 1.0f, 1.0f, 1.0f},
                        true
                );
            }
            if (j < m_rows - 1) {
                Debug::drawLine(
                        {i * WORLD_SCALE, (j - (float)(int)m_grid.at(i).at(j) * ELEVATION_HEIGHT) * WORLD_SCALE},
                        {i * WORLD_SCALE, (j - (float)(int)m_grid.at(i).at(j + 1) * ELEVATION_HEIGHT + 1) * WORLD_SCALE},
                        {1.0f, 1.0f, 1.0f, 1.0f},
                        true
                );
            }
        }
    }
}

bool ElevationGrid::setElevation(glm::ivec2 gridPos, Elevation elevation) {
    if (!canElevate(gridPos, elevation)) return false;

    // Flatten surrounding terrain
    if (elevation != Elevation::Flat) {
        auto pointsToFlatten = getAdjacentGridPositions(gridPos, true);
        auto end = std::remove_if(pointsToFlatten.begin(), pointsToFlatten.end(), [this, elevation](auto pos) { return (int)getElevationAtGridPos(pos) != -(int)elevation; });
        auto failedToFlatten = false;
        for (auto point = pointsToFlatten.begin(); point != end; ++point) {
            if (!setElevation(*point, Elevation::Flat)) failedToFlatten = true;
        }
        if (failedToFlatten) return false;
    }

    m_grid.at(gridPos.x).at(gridPos.y) = elevation;
    return true;
}

void ElevationGrid::setElevationInCircle(glm::vec2 pos, float radius, Elevation elevation) {
    if (!isPositionInGrid(pos)) return;

    bool changed = false;

    std::vector<glm::vec2> points;

    for (float i = pos.x - radius; i <= pos.x + radius; i++) {
        for (float j = pos.y - radius; j <= pos.y + radius; j++) {
            auto gridPos = glm::floor(glm::vec2{i, j});
            if (!isPositionInGrid(gridPos)) continue;

            if (pointInCircle(pos, radius, gridPos)) {
                if (getElevationAtGridPos(gridPos) != elevation) {
                    points.push_back(gridPos);
                    changed = true;
                }
            }
        }
    }

    // Can we elevate tile objects that require flat terrain?
    // Get each elevate point
    // Collect the flatten points of each elevate point
    // Attempt to flatten and record results
    //     Recursive check for adjacent required points and see if they're also being flattened
    // For each elevate point check flatten results
    //     If all succeeded then
    //     Recursive check for adjacent required points being elevated
    //     elevate point

    // TODO: Figure out how to do path adjacency requirements (2 adjacent points or all 4)
    // What if a 4 banger raises a path but an adjacent path is a 3 banger?
    // Fuck my ass
    // Collect a list of all recursive points and then check each path around the point? How does that interact with gates and shit

    // This might be more complex than its worth tbh

    if (!changed) return;

    for (auto point : points) {
        setElevation(point, elevation);
    }

    // Regenerate meshes
    Zoo::zoo->m_world->m_biomeGrid->redrawChunksInRadius(pos * 2.0f, radius + 6.0f);
    generateWaterMesh();
}

void ElevationGrid::generateWaterMesh() {
    std::vector<glm::vec2> vertices{};
    std::vector<unsigned int> indices{};

    for (int i = 0; i < m_cols - 1; ++i) {
        for (int j = 0; j < m_rows - 1; ++j) {
            if (getTileBaseElevation({i, j}) < 0) {
                auto index = vertices.size();
                auto tile = getTileWaterVertices({i, j});
                tile.insert(tile.begin(), {0.5f, 0.5f});
                for (auto v : tile) {
                    vertices.push_back({i + v.x, j + v.y});
                    indices.push_back(index);
                    index++;
                }
                indices.push_back(index - tile.size() + 1);
                indices.push_back(PRIMITIVE_RESTART);
            }
        }
    }
    m_numIndices = indices.size();

    m_vb = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(float) * 2);
    m_ib = std::make_unique<IndexBuffer>(&indices[0], indices.size());

    m_va->addBuffer(*m_vb, *m_layout);
}

bool ElevationGrid::canElevate(glm::ivec2 gridPos, Elevation elevation) {
    return true;
}

Elevation ElevationGrid::getElevationAtGridPos(glm::ivec2 gridPos) {
    if (m_grid.empty()) return Elevation::Flat;
    if (!isPositionInGrid(gridPos)) return Elevation::Flat;

    return m_grid.at(gridPos.x).at(gridPos.y);
}

float ElevationGrid::getTileBaseElevation(glm::ivec2 tile) {
    auto a = ((float)std::min({
         (int)getElevationAtGridPos(tile),
         (int)getElevationAtGridPos(tile + glm::ivec2{0, 1}),
         (int)getElevationAtGridPos(tile + glm::ivec2{1, 0}),
         (int)getElevationAtGridPos(tile + glm::ivec2{1, 1})
     })) * ELEVATION_HEIGHT;

    return a;
}

SlopeVariant ElevationGrid::getTileSlopeVariant(glm::ivec2 tile) {
    auto nw = getElevationAtGridPos(tile);
    auto ne = getElevationAtGridPos(tile + glm::ivec2{1, 0});
    auto sw = getElevationAtGridPos(tile + glm::ivec2{0, 1});
    auto se = getElevationAtGridPos(tile + glm::ivec2{1, 1});

    if (nw == ne && nw == sw && nw == se) return SlopeVariant::Flat;
    if (nw == ne && sw == se && nw < sw) return SlopeVariant::N;
    if (nw == sw && ne == se && nw > ne) return SlopeVariant::E;
    if (nw == ne && sw == se && nw > sw) return SlopeVariant::S;
    if (nw == sw && ne == se && nw < ne) return SlopeVariant::W;
    if (se == sw && se == ne && nw > se) return SlopeVariant::SE;
    if (sw == nw && sw == se && ne > sw) return SlopeVariant::SW;
    if (ne == nw && ne == se && sw > ne) return SlopeVariant::NE;
    if (nw == sw && nw == ne && se > nw) return SlopeVariant::NW;
    if (se == sw && se == ne && nw < se) return SlopeVariant::INW;
    if (sw == nw && sw == se && ne < sw) return SlopeVariant::INE;
    if (ne == nw && ne == se && sw < ne) return SlopeVariant::ISW;
    if (nw == sw && nw == ne && se < nw) return SlopeVariant::ISE;
    if (nw == se && sw == ne && nw > ne) return SlopeVariant::I1;
    if (nw == se && sw == ne && nw < ne) return SlopeVariant::I2;

    // How did you get here?
    return SlopeVariant::Flat;
}

std::vector<glm::vec2> ElevationGrid::getTileWaterVertices(glm::ivec2 gridPos) {
    auto slopeVariant = getTileSlopeVariant(gridPos);
    std::vector<glm::vec2> vertices{};

    switch (slopeVariant) {
        case SlopeVariant::N:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f - WATER_LEVEL);
            vertices.emplace_back(0.0f, 1.0f - WATER_LEVEL);
            break;
        case SlopeVariant::S:
            vertices.emplace_back(0.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::W:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::E:
            vertices.emplace_back(WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(WATER_LEVEL, 1.0f);
            break;
        case SlopeVariant::NW:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f - WATER_LEVEL);
            vertices.emplace_back(1.0f - WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::NE:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f - WATER_LEVEL);
            break;
        case SlopeVariant::SW:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::SE:
            vertices.emplace_back(0.0f, WATER_LEVEL);
            vertices.emplace_back(WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::INW:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 1.0f - WATER_LEVEL);
            vertices.emplace_back(0.0f, 1.0f - WATER_LEVEL);
            break;
        case SlopeVariant::INE:
            vertices.emplace_back(WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f - WATER_LEVEL);
            vertices.emplace_back(WATER_LEVEL, 1.0f - WATER_LEVEL);
            break;
        case SlopeVariant::ISW:
            vertices.emplace_back(0.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f - WATER_LEVEL, WATER_LEVEL);
            vertices.emplace_back(1.0f - WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::ISE:
            vertices.emplace_back(WATER_LEVEL, WATER_LEVEL);
            vertices.emplace_back(1.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(WATER_LEVEL, 1.0f);
            break;
        case SlopeVariant::I1:
            vertices.emplace_back(0.0f, WATER_LEVEL);
            vertices.emplace_back(WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f - WATER_LEVEL);
            vertices.emplace_back(1.0f - WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        case SlopeVariant::I2:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f - WATER_LEVEL, 0.0f);
            vertices.emplace_back(1.0f, WATER_LEVEL);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(WATER_LEVEL, 1.0f);
            vertices.emplace_back(0.0f, 1.0f - WATER_LEVEL);
            break;
        case SlopeVariant::Flat:
            vertices.emplace_back(0.0f, 0.0f);
            vertices.emplace_back(1.0f, 0.0f);
            vertices.emplace_back(1.0f, 1.0f);
            vertices.emplace_back(0.0f, 1.0f);
            break;
        default:
            break;
    }

    for (auto& vertex : vertices) {
        vertex.y += WATER_LEVEL * ELEVATION_HEIGHT;
    }

    return vertices;
}

float ElevationGrid::getElevationAtPos(glm::vec2 pos) {
    if (!isPositionInGrid(pos)) {
        return 0;
    }

    float relX = (float)fmod(pos.x, 1);
    float relY = (float)fmod(pos.y, 1);
    float baseElevation = (float)getTileBaseElevation(glm::floor(pos));
    auto slopeVariant = getTileSlopeVariant(glm::floor(pos));

    // Tried to come up with a formula instead of using enums but I'm too dumb
    switch (slopeVariant) {
        case SlopeVariant::Flat:
            return baseElevation;
        case SlopeVariant::N:
            return baseElevation + ELEVATION_HEIGHT * relY;
        case SlopeVariant::S:
            return baseElevation + ELEVATION_HEIGHT * (1 - relY);
        case SlopeVariant::W:
            return baseElevation + ELEVATION_HEIGHT * relX;
        case SlopeVariant::E:
            return baseElevation + ELEVATION_HEIGHT * (1 - relX);
        case SlopeVariant::SE:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - relX - relY, 0.0f);
        case SlopeVariant::SW:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - (1 - relX) - relY, 0.0f);
        case SlopeVariant::NE:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - relX - (1 - relY), 0.0f);
        case SlopeVariant::NW:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - (1 - relX) - (1 - relY), 0.0f);
        case SlopeVariant::ISE:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - relX, 1 - relY);
        case SlopeVariant::ISW:
            return baseElevation + ELEVATION_HEIGHT * glm::max(relX, 1 - relY);
        case SlopeVariant::INE:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - relX, relY);
        case SlopeVariant::INW:
            return baseElevation + ELEVATION_HEIGHT * glm::max(relX, relY);
        case SlopeVariant::I1:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - relX - relY, 1 - (1 - relX) - (1 - relY));
        case SlopeVariant::I2:
            return baseElevation + ELEVATION_HEIGHT * glm::max(1 - (1 - relX) - relY, 1 - relX - (1 - relY));
        default:
            // You shouldn't be here
            return 0;
    }
}

bool ElevationGrid::isPositionInGrid(glm::vec2 pos) const {
    return pos.x >= 0 && pos.x < (float)m_cols && pos.y >= 0 && pos.y < (float)m_rows;
}

bool ElevationGrid::isPositionSloped(glm::vec2 pos) {
    return getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::Flat;
}

bool ElevationGrid::isPositionSlopeCorner(glm::vec2 pos) {
    return (
        getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::Flat &&
        getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::N &&
        getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::S &&
        getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::E &&
        getTileSlopeVariant(glm::floor(pos)) != SlopeVariant::W
    );
}

bool ElevationGrid::isPositionWater(glm::vec2 pos) {
    return isTileWater(glm::floor(pos));
}

bool ElevationGrid::isTileWater(glm::ivec2 pos) {
    return m_grid.at(pos.x).at(pos.y) == Elevation::Water;
}

std::vector<glm::ivec2> ElevationGrid::getAdjacentGridPositions(glm::ivec2 gridPos) {
    return getAdjacentGridPositions(gridPos, false);
}

std::vector<glm::ivec2> ElevationGrid::getAdjacentGridPositions(glm::ivec2 gridPos, bool diagonals) {
    auto positions = std::vector<glm::ivec2>{
        gridPos + glm::ivec2{1, 0},
        gridPos + glm::ivec2{-1, 0},
        gridPos + glm::ivec2{0, 1},
        gridPos + glm::ivec2{0, -1}
    };
    if (diagonals) {
        positions.emplace_back(gridPos + glm::ivec2{1, -1});
        positions.emplace_back(gridPos + glm::ivec2{1, 1});
        positions.emplace_back(gridPos + glm::ivec2{-1, 1});
        positions.emplace_back(gridPos + glm::ivec2{-1, -1});
    }

    return positions;
}

json ElevationGrid::save() {
    return json{
        {"grid", m_grid}
    };
}

void ElevationGrid::load(json saveData) {
    reset();

    m_grid = saveData["grid"].get<std::vector<std::vector<Elevation>>>();
    m_cols = m_grid.size();
    m_rows = m_grid[0].size();

    generateWaterMesh();
}
