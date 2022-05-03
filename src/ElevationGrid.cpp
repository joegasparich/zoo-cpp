#include "ElevationGrid.h"
#include "constants/world.h"
#include "util/math.h"
#include "Game.h"
#include "Debug.h"

ElevationGrid::ElevationGrid(unsigned int rows, unsigned int cols) : m_rows(rows), m_cols(cols) {}

void ElevationGrid::setup() {
    for (unsigned int i = 0; i < m_cols; i++) {
        m_grid.emplace_back();
        for (unsigned int j = 0; j < m_rows; j++) {
            m_grid.at(i).push_back(Elevation::Flat);
        }
    }
}

void ElevationGrid::setElevation(glm::ivec2 gridPos, Elevation elevation) {
    if (!canElevate(gridPos, elevation)) return;

    // Flatten surrounding terrain
//    if (elevation != Elevation::Flat) {
//        auto flattened = getAdjacentGridPositions(gridPos, true);
//        std::remove_if(flattened.begin(), flattened.end(), [this, gridPos, elevation](auto pos) { return (int)getElevationAtGridPos(gridPos) == -(int)elevation; })
//    }

    m_grid.at(gridPos.x).at(gridPos.y) = elevation;
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

    if (!changed) return;

    for (auto point : points) {
        setElevation(point, elevation);
    }

    // Redraw biome grid
    Game::get().m_stage->m_world->m_biomeGrid->redrawChunksInRadius(pos * 2.0f, radius + 5.0f);
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

std::vector<glm::vec2> ElevationGrid::getSurroundingTiles(glm::ivec2 gridPos) {
    return std::vector<glm::vec2>();
}

std::vector<glm::vec2> ElevationGrid::getAdjacentGridPositions(glm::ivec2 gridPos) {
    return std::vector<glm::vec2>();
}

std::vector<glm::vec2> ElevationGrid::getAdjacentGridPositions(glm::ivec2 gridPos, bool diagonals) {
    auto positions = std::vector<glm::vec2>{
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

void ElevationGrid::renderDebug() {
    auto& input = Game::get().m_input;
    auto& renderer = Renderer::get();
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
