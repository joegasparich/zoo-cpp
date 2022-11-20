#include <entities/components/TileObjectComponent.h>
#include "ElevationGrid.h"
#include "constants/world.h"
#include "Debug.h"
#include "ZooScene.h"
#include "Messenger.h"

#define WATER_LEVEL 0.1f
#define WATER_COLOR {0, 76, 255, 204}

ElevationGrid::ElevationGrid(unsigned int rows, unsigned int cols) : rows(rows), cols(cols) {}

void ElevationGrid::setup() {
    for (unsigned int i = 0; i < cols; i++) {
        grid.emplace_back();
        for (unsigned int j = 0; j < rows; j++) {
            grid.at(i).push_back(Elevation::Flat);
        }
    }

    generateWaterMesh();
}

void ElevationGrid::cleanup() {
    cols = 0;
    rows = 0;
    grid.clear();
}

void ElevationGrid::render() {
    for(auto poly : polygons) {
        DrawTriangleFan(&poly[0], poly.size(), WATER_COLOR);
    }
}

void ElevationGrid::renderDebug() {
    if (grid.empty()) return;

    for (auto i = 0; i < cols; i++) {
        for (auto j = 0; j < rows; j++) {
            if (i < cols - 1) {
                // Double cast converts enum to int then to float
                Debug::drawLine(
                    {float(i), j - int(grid.at(i).at(j)) * ELEVATION_HEIGHT},
                    {float(i + 1), j - int(grid.at(i + 1).at(j)) * ELEVATION_HEIGHT},
                    {255, 255, 255, 255},
                    true
                );
            }

            if (j < rows - 1) {
                Debug::drawLine(
                    {float(i), (j - int(grid.at(i).at(j)) * ELEVATION_HEIGHT)},
                    {float(i), (j - int(grid.at(i).at(j + 1)) * ELEVATION_HEIGHT + 1)},
                    {255, 255, 255, 255},
                    true
                );
            }
        }
    }
}

bool ElevationGrid::setElevation(Cell gridPos, Elevation elevation) {
    if (!canElevate(gridPos, elevation)) return false;

    // Flatten surrounding terrain
    if (elevation != Elevation::Flat) {
        auto pointsToFlatten = getAdjacentGridPositions(gridPos, true);
        auto end = std::remove_if(pointsToFlatten.begin(), pointsToFlatten.end(), [this, elevation](auto pos) { return int(getElevationAtGridPos(pos)) != -int(elevation); });
        auto failedToFlatten = false;
        for (auto point = pointsToFlatten.begin(); point != end; ++point) {
            if (!setElevation(*point, Elevation::Flat)) failedToFlatten = true;
        }
        if (failedToFlatten) return false;
    }

    grid.at(gridPos.x).at(gridPos.y) = elevation;
    return true;
}

void ElevationGrid::setElevationInCircle(Vector2 pos, float radius, Elevation elevation) {
    if (!isPositionInGrid(pos)) return;

    bool changed = false;

    std::vector<Cell> points;

    for (float i = pos.x - radius; i <= pos.x + radius; i++) {
        for (float j = pos.y - radius; j <= pos.y + radius; j++) {
            auto gridPos = floor({i, j});
            if (!isPositionInGrid(gridPos)) continue;

            if (pointInCircle(pos, radius, gridPos)) {
                if (getElevationAtGridPos(gridPos) != elevation) {
                    points.push_back(gridPos);
                    changed = true; // TODO: Check canElevate to avoid heaps of mesh regeneration
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

    generateWaterMesh();

    Messenger::fire(EventType::ElevationUpdated, json{{"pos", pos}, {"radius", radius}});
}

void ElevationGrid::generateWaterMesh() {
    polygons.clear();

    for (int i = 0; i < cols - 1; ++i) {
        for (int j = 0; j < rows - 1; ++j) {
            if (getTileBaseElevation({i, j}) < 0) {
                auto tile = getTileWaterVertices({i, j});
                Vector2 average = {0, 0};
                auto poly = std::vector<Vector2>{};
                for (auto v : tile) {
                    poly.push_back({(i + v.x) * WORLD_SCALE, (j + v.y) * WORLD_SCALE});
                    average += poly.back();
                }
                // Centre
                poly.insert(poly.begin(), {average.x/poly.size(), average.y/poly.size()});
                // Join start and end
                poly.push_back(poly[1]);


                polygons.push_back(poly);
            }
        }
    }
}

bool ElevationGrid::canElevate(Cell gridPos, Elevation elevation) {
    if (elevation == Elevation::Water) {
        // Check 4 surrounding wall slots for walls
        for (auto wall : Root::zoo()->world->wallGrid->getSurroundingWalls(gridPos)) {
            if (wall->exists) return false;
        }
        // Check 4 surrounding tiles
        for (auto tile : getSurroundingTiles(gridPos)) {
            // Check for Tile Objects
            auto entity = Root::zoo()->world->getTileObjectAtPosition(tile);
            if (entity) {
                auto tileObject = entity->getComponent<TileObjectComponent>();
                if (tileObject && !tileObject->m_data.canPlaceInWater) return false;
            }
            // Check for paths
            if (Root::zoo()->world->pathGrid->getPathAtTile(tile)->exists) return false;
        }
    }
    if (elevation == Elevation::Hill) {
        // Check 4 surrounding tiles for tile objects
        for (auto tile : getSurroundingTiles(gridPos)) {
            if (!Root::zoo()->world->isPositionInMap(tile)) continue;

            auto entity = Root::zoo()->world->getTileObjectAtPosition(tile);
            if (entity) {
                auto tileObject = entity->getComponent<TileObjectComponent>();
                if (tileObject && !tileObject->m_data.canPlaceOnSlopes) return false;
            }
            // Check for paths
            // TODO: Figure out if points are being elevated in a way where this is valid
            if (Root::zoo()->world->pathGrid->getPathAtTile(tile)->exists) return false;
        }
    }

    return true;
}

Elevation ElevationGrid::getElevationAtGridPos(Cell gridPos) {
    if (grid.empty()) return Elevation::Flat;
    if (!isPositionInGrid(gridPos)) return Elevation::Flat;

    return grid.at(gridPos.x).at(gridPos.y);
}

float ElevationGrid::getTileBaseElevation(Cell tile) {
    auto a = (float(std::min({
         int(getElevationAtGridPos(tile)),
         int(getElevationAtGridPos(tile + Cell{0, 1})),
         int(getElevationAtGridPos(tile + Cell{1, 0})),
         int(getElevationAtGridPos(tile + Cell{1, 1}))
     }))) * ELEVATION_HEIGHT;

    return a;
}

SlopeVariant ElevationGrid::getTileSlopeVariant(Cell tile) {
    auto nw = getElevationAtGridPos(tile);
    auto ne = getElevationAtGridPos(tile + Cell{1, 0});
    auto sw = getElevationAtGridPos(tile + Cell{0, 1});
    auto se = getElevationAtGridPos(tile + Cell{1, 1});

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

std::vector<Vector2> ElevationGrid::getTileWaterVertices(Cell gridPos) {
    auto slopeVariant = getTileSlopeVariant(gridPos);
    std::vector<Vector2> vertices{};

    switch (slopeVariant) {
        case SlopeVariant::N:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f, 0.0f});
            break;
        case SlopeVariant::S:
            vertices.push_back({0.0f, WATER_LEVEL});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, WATER_LEVEL});
            break;
        case SlopeVariant::W:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f - WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f - WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::E:
            vertices.push_back({WATER_LEVEL, 0.0f});
            vertices.push_back({WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, 0.0f});
            break;
        case SlopeVariant::NW:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f - WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f, 0.0f});
            break;
        case SlopeVariant::NE:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, 0.0f});
            break;
        case SlopeVariant::SW:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, WATER_LEVEL});
            vertices.push_back({1.0f - WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::SE:
            vertices.push_back({0.0f, WATER_LEVEL});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, 0.0f});
            vertices.push_back({WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::INW:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f - WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::INE:
            vertices.push_back({WATER_LEVEL, 0.0f});
            vertices.push_back({1.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f, 0.0f});
            break;
        case SlopeVariant::ISW:
            vertices.push_back({0.0f, WATER_LEVEL});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f - WATER_LEVEL, 1.0f});
            break;
        case SlopeVariant::ISE:
            vertices.push_back({WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, WATER_LEVEL});
            break;
        case SlopeVariant::I1:
            vertices.push_back({0.0f, WATER_LEVEL});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f - WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({1.0f, 0.0f});
            vertices.push_back({WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::I2:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f - WATER_LEVEL});
            vertices.push_back({WATER_LEVEL, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, WATER_LEVEL});
            vertices.push_back({1.0f - WATER_LEVEL, 0.0f});
            break;
        case SlopeVariant::Flat:
            vertices.push_back({0.0f, 0.0f});
            vertices.push_back({0.0f, 1.0f});
            vertices.push_back({1.0f, 1.0f});
            vertices.push_back({1.0f, 0.0f});
            break;
        default:
            break;
    }

    for (auto& vertex : vertices) {
        vertex.y += WATER_LEVEL * ELEVATION_HEIGHT;
    }

    return vertices;
}

float ElevationGrid::getElevationAtPos(Vector2 pos) {
    if (!isPositionInGrid(pos)) {
        return 0;
    }

    float relX = float(fmod(pos.x, 1));
    float relY = float(fmod(pos.y, 1));
    float baseElevation = float(getTileBaseElevation(floor(pos)));
    auto slopeVariant = getTileSlopeVariant(floor(pos));

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
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - relX - relY, 0.0f);
        case SlopeVariant::SW:
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - (1 - relX) - relY, 0.0f);
        case SlopeVariant::NE:
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - relX - (1 - relY), 0.0f);
        case SlopeVariant::NW:
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - (1 - relX) - (1 - relY), 0.0f);
        case SlopeVariant::ISE:
            return baseElevation + ELEVATION_HEIGHT * std::min(1 - relX + 1 - relY, 1.0f);
        case SlopeVariant::ISW:
            return baseElevation + ELEVATION_HEIGHT * std::min(relX + 1 - relY, 1.0f);
        case SlopeVariant::INE:
            return baseElevation + ELEVATION_HEIGHT * std::min(1 - relX + relY, 1.0f);
        case SlopeVariant::INW:
            return baseElevation + ELEVATION_HEIGHT * std::min(relX + relY, 1.0f);
        case SlopeVariant::I1:
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - relX - relY, 1 - (1 - relX) - (1 - relY));
        case SlopeVariant::I2:
            return baseElevation + ELEVATION_HEIGHT * std::max(1 - (1 - relX) - relY, 1 - relX - (1 - relY));
        default:
            // You shouldn't be here
            assert(false);
            return 0;
    }
}

std::vector<Cell> ElevationGrid::getSurroundingTiles(Cell gridPos) {
    return {
        gridPos + Cell{-1, -1},
        gridPos + Cell{-1, 0},
        gridPos + Cell{0, -1},
        gridPos + Cell{0, 0},
    };
}

bool ElevationGrid::isPositionInGrid(Vector2 pos) const {
    return pos.x >= 0 && pos.x < float(cols) && pos.y >= 0 && pos.y < float(rows);
}

bool ElevationGrid::isPositionSloped(Vector2 pos) {
    return getTileSlopeVariant(floor(pos)) != SlopeVariant::Flat;
}

bool ElevationGrid::isPositionSlopeCorner(Vector2 pos) {
    return (
        getTileSlopeVariant(floor(pos)) != SlopeVariant::Flat &&
        getTileSlopeVariant(floor(pos)) != SlopeVariant::N &&
        getTileSlopeVariant(floor(pos)) != SlopeVariant::S &&
        getTileSlopeVariant(floor(pos)) != SlopeVariant::E &&
        getTileSlopeVariant(floor(pos)) != SlopeVariant::W
    );
}

bool ElevationGrid::isPositionWater(Vector2 pos) {
    return isTileWater(floor(pos));
}

bool ElevationGrid::isTileWater(Cell pos) {
    if (!isPositionInGrid(pos)) return false;
    return getTileBaseElevation(pos) < 0;
}

std::vector<Cell> ElevationGrid::getAdjacentGridPositions(Cell gridPos) {
    return getAdjacentGridPositions(gridPos, false);
}

std::vector<Cell> ElevationGrid::getAdjacentGridPositions(Cell gridPos, bool diagonals) {
    auto positions = std::vector<Cell>{
        gridPos + Cell{1, 0},
        gridPos + Cell{-1, 0},
        gridPos + Cell{0, 1},
        gridPos + Cell{0, -1}
    };
    if (diagonals) {
        positions.emplace_back(gridPos + Cell{1, -1});
        positions.emplace_back(gridPos + Cell{1, 1});
        positions.emplace_back(gridPos + Cell{-1, 1});
        positions.emplace_back(gridPos + Cell{-1, -1});
    }

    return positions;
}

json ElevationGrid::save() {
    return json{
        {"grid", grid}
    };
}

void ElevationGrid::load(json saveData) {
    cleanup();

    saveData.at("grid").get_to(grid);
    cols = grid.size();
    rows = grid[0].size();

    generateWaterMesh();
}
