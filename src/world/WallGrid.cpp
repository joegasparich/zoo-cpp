#include "ZooScene.h"
#include "util/util.h"
#include "constants/assets.h"
#include "WallGrid.h"
#include "constants/world.h"
#include "Messenger.h"

WallGrid::WallGrid(unsigned int cols, unsigned int rows) : cols(cols), rows(rows) {}

void WallGrid::setup() {
    auto walls = Registry::getAllWalls();

    for (int i = 0; i < cols * 2 + 1; i++) {
        auto orientation = (Orientation)(i % 2);
        grid.emplace_back();
        for (int j = 0; j < rows + int(orientation); j++) {
            auto worldPos = WallGrid::wallToWorldPosition({i, j}, orientation);
            grid.at(i).push_back({
                nullptr,
                orientation,
                worldPos,
                {i, j},
                false,
                false
            });
        }
    }

    isSetup = true;
}

void WallGrid::cleanup() {
    grid.clear();
    cols = 0;
    rows = 0;


    isSetup = false;
}

void WallGrid::render() {
    for (int i = 0; i < cols * 2 + 1; i++) {
        auto orientation = (Orientation)(i % 2);
        for (int j = 0; j < rows + int(orientation); j++) {
            auto& wall = grid.at(i).at(j);
            if (wall.exists) {
                auto [ spriteIndex, elevation ] = WallGrid::getSpriteInfo(wall);
                auto pos = orientation == Orientation::Horizontal ? Vector2{i / 2.0f, float(j)} : Vector2{i / 2.0f, float(j + 1)};
                pos -= Vector2{0.5f, 2.0f + elevation}; // Offset cell size
                auto spritesheet = Root::assetManager().getSpriteSheet(wall.data->spriteSheetPath);

                BlitOptions opts;
                opts.texture = spritesheet->texture;
                opts.source = spritesheet->getCellBounds(spriteIndex);
                opts.pos = pos * WORLD_SCALE;
                opts.depth = Root::renderer().getDepth(wall.worldPos.y);
                opts.scale = Vector2{1, 2} * WORLD_SCALE;

                Root::renderer().blit(opts);
            }
        }
    }
}

Wall* WallGrid::placeWallAtTile(WallData* data, Cell tilePos, Side side) {
    assert(isSetup);
    if (!isWallPosInMap(tilePos, side)) return nullptr;
    if (getWallAtTile(tilePos, side)->exists) return nullptr;

    auto [ x, y, orientation ] = WallGrid::getGridPosition(tilePos, side);

    grid.at(x).at(y) = Wall{
            data,
            orientation,
            WallGrid::wallToWorldPosition({x, y}, orientation),
            {x, y},
            true,
            false
    };

    auto& wall = grid.at(x).at(y);

    updatePathfindingAtWall(wall);

    if (shouldCheckForLoop(wall) && checkForLoop(&wall)) {
        Root::zoo()->world->formAreas(grid.at(x).at(y));
    }

    return &wall;
}

void WallGrid::deleteWall(Wall wall) {
    assert(isSetup);

    this->deleteWallAtTile(
        flr(wall.worldPos),
        wall.orientation == Orientation::Horizontal ? Side::North : Side::West
    );
}

void WallGrid::deleteWallAtTile(Cell tilePos, Side side) {
    assert(isSetup);
    if (!isWallPosInMap(tilePos, side)) return;

    // Get grid position
    auto [ x, y, orientation ] = WallGrid::getGridPosition(tilePos, side);
    if (!grid[x][y].exists || grid[x][y].indestructable) return;

    // Set to blank wall
    grid[x][y] = Wall{
            {},
            orientation,
            WallGrid::wallToWorldPosition({x, y}, orientation),
            {x, y},
            false,
            false
    };
}

void WallGrid::placeDoor(Wall* wall) {
    assert(isSetup);
    if (!wall) return;

    wall->isDoor = true;

    auto adjacentTiles = getAdjacentTiles(*wall);
    if (adjacentTiles.size() < 2) return;

    auto areaA = Root::zoo()->world->getAreaAtPosition(adjacentTiles[0]);
    auto areaB = Root::zoo()->world->getAreaAtPosition(adjacentTiles[1]);

    areaA->addAreaConnection(areaB, wall);
    areaB->addAreaConnection(areaA, wall);
}

void WallGrid::removeDoor(Wall* wall) {
    assert(isSetup);
    wall->isDoor = false;

    auto adjacentTiles = getAdjacentTiles(*wall);
    if (adjacentTiles.size() < 2) return;

    auto areaA = Root::zoo()->world->getAreaAtPosition(adjacentTiles[0]);
    auto areaB = Root::zoo()->world->getAreaAtPosition(adjacentTiles[1]);
    
    areaA->removeAreaConnection(areaB, wall);
    areaB->removeAreaConnection(areaA, wall);
}

bool WallGrid::isWallPosInMap(Cell tilePos, Side side) const {
    assert(isSetup);

    return (
        Root::zoo()->world->isPositionInMap(tilePos) ||
        (Root::zoo()->world->isPositionInMap(tilePos + Cell{0, 1}) && side == Side::South) ||
        (Root::zoo()->world->isPositionInMap(tilePos + Cell{0, -1}) && side == Side::North) ||
        (Root::zoo()->world->isPositionInMap(tilePos + Cell{1, 0}) && side == Side::East) ||
        (Root::zoo()->world->isPositionInMap(tilePos + Cell{-1, 0}) && side == Side::West)
    );
}

bool WallGrid::isWallGridPosInMap(Cell gridPos) {
    assert(isSetup);

    return gridPos.x >= 0 && gridPos.x < grid.size() && gridPos.y >= 0 && gridPos.y < grid[gridPos.x].size();
}

Wall* WallGrid::getWallAtTile(Cell tilePos, Side side) {
    assert(isSetup);

    if (!isWallPosInMap(tilePos, side)) {
        // Invert position and side if tile pos is correct but on the outside of the map
        if (Root::zoo()->world->isPositionInMap(tilePos + Cell(0, 1)) && side == Side::South) {
            tilePos = tilePos + Cell(0, 1);
            side = Side::North;
        } else if (Root::zoo()->world->isPositionInMap(tilePos + Cell(0, -1)) && side == Side::North) {
            tilePos = tilePos + Cell(0, -1);
            side = Side::South;
        } else if (Root::zoo()->world->isPositionInMap(tilePos + Cell(1, 0)) && side == Side::East) {
            tilePos = tilePos + Cell(-1, 0);
            side = Side::West;
        } else if (Root::zoo()->world->isPositionInMap(tilePos + Cell(-1, 0)) && side == Side::West) {
            tilePos = tilePos + Cell(1, 0);
            side = Side::East;
        } else {
            // Position is outside map
            return nullptr;
        }
    }

    switch (side) {
        case Side::North:
            return &grid[tilePos.x * 2 + 1][tilePos.y];
        case Side::East:
            return &grid[tilePos.x * 2 + 2][tilePos.y];
        case Side::South:
            return &grid[tilePos.x * 2 + 1][tilePos.y + 1];
        case Side::West:
            return &grid[tilePos.x * 2][tilePos.y];
    }
}

void WallGrid::updatePathfindingAtWall(const Wall& wall) {
    assert(isSetup);
    auto [x, y] = wall.worldPos;
    auto& pf = Root::zoo()->world->pathfinder;

    if (wall.orientation == Orientation::Horizontal) {
        if (Root::zoo()->world->isPositionInMap({x - 0.5f, y - 1.0f})) {
            auto north = round({x - 0.5f, y - 1.0f});
            pf->setAccessibility(north, Direction::S, !wall.exists);
            pf->setAccessibility(north, Direction::SE, !wall.exists);
            pf->setAccessibility(north, Direction::SW, !wall.exists);
            pf->setAccessibility(north + Cell{-1, 0}, Direction::SE, !wall.exists);
            pf->setAccessibility(north + Cell{1, 0}, Direction::SW, !wall.exists);
        }
        if (Root::zoo()->world->isPositionInMap({x - 0.5f, y})) {
            auto south = round({x - 0.5f, y});
            pf->setAccessibility(south, Direction::N, !wall.exists);
            pf->setAccessibility(south, Direction::NE, !wall.exists);
            pf->setAccessibility(south, Direction::NW, !wall.exists);
            pf->setAccessibility(south + Cell{-1, 0}, Direction::NE, !wall.exists);
            pf->setAccessibility(south + Cell{1, 0}, Direction::NW, !wall.exists);
        }
    } else {
        if (Root::zoo()->world->isPositionInMap({x - 1.0f, y - 0.5f})) {
            auto west = round({x - 1.0f, y - 0.5f});
            pf->setAccessibility(west, Direction::E, !wall.exists);
            pf->setAccessibility(west, Direction::NE, !wall.exists);
            pf->setAccessibility(west, Direction::SE, !wall.exists);
            pf->setAccessibility(west + Cell{0, -1}, Direction::SE, !wall.exists);
            pf->setAccessibility(west + Cell{0, 1}, Direction::NE, !wall.exists);
        }
        if (Root::zoo()->world->isPositionInMap({x, y - 0.5f})) {
            auto east = round({x, y - 0.5f});
            pf->setAccessibility(east, Direction::W, !wall.exists);
            pf->setAccessibility(east, Direction::NW, !wall.exists);
            pf->setAccessibility(east, Direction::SW, !wall.exists);
            pf->setAccessibility(east + Cell{0, -1}, Direction::SW, !wall.exists);
            pf->setAccessibility(east + Cell{0, 1}, Direction::NW, !wall.exists);
        }
    }
}

Wall* WallGrid::getWallByGridPos(Cell gridPos) {
    assert(isSetup);
    if (!isWallGridPosInMap(gridPos)) return nullptr;

    return &grid[gridPos.x][gridPos.y];
}

std::vector<Wall*> WallGrid::getAdjacentWalls(const Wall &wall) {
    assert(isSetup);

    std::vector<Wall*> adjacentWalls{};
    auto x = wall.gridPos.x; auto y = wall.gridPos.y;

    if (wall.orientation == Orientation::Horizontal) {
        if (getWallByGridPos({x - 2, y}) && getWallByGridPos({x - 2, y})->exists) adjacentWalls.push_back(&grid[x - 2][y]);
        if (getWallByGridPos({x + 2, y}) && getWallByGridPos({x + 2, y})->exists) adjacentWalls.push_back(&grid[x + 2][y]);
        if (getWallByGridPos({x - 1, y}) && getWallByGridPos({x - 1, y})->exists) adjacentWalls.push_back(&grid[x - 1][y]);
        if (getWallByGridPos({x + 1, y}) && getWallByGridPos({x + 1, y})->exists) adjacentWalls.push_back(&grid[x + 1][y]);
        if (getWallByGridPos({x - 1, y - 1}) && getWallByGridPos({x - 1, y - 1})->exists) adjacentWalls.push_back(&grid[x - 1][y - 1]);
        if (getWallByGridPos({x + 1, y - 1}) && getWallByGridPos({x + 1, y - 1})->exists) adjacentWalls.push_back(&grid[x + 1][y - 1]);
    } else {
        if (getWallByGridPos({x - 1, y}) && getWallByGridPos({x - 1, y})->exists) adjacentWalls.push_back(&grid[x - 1][y]);
        if (getWallByGridPos({x + 1, y}) && getWallByGridPos({x + 1, y})->exists) adjacentWalls.push_back(&grid[x + 1][y]);
        if (getWallByGridPos({x - 1, y + 1}) && getWallByGridPos({x - 1, y + 1})->exists) adjacentWalls.push_back(&grid[x - 1][y + 1]);
        if (getWallByGridPos({x + 1, y + 1}) && getWallByGridPos({x + 1, y + 1})->exists) adjacentWalls.push_back(&grid[x + 1][y + 1]);
        if (getWallByGridPos({x, y + 1}) && getWallByGridPos({x, y + 1})->exists) adjacentWalls.push_back(&grid[x][y + 1]);
        if (getWallByGridPos({x, y - 1}) && getWallByGridPos({x, y - 1})->exists) adjacentWalls.push_back(&grid[x][y - 1]);
    }

    return adjacentWalls;
}

std::vector<Cell> WallGrid::getAdjacentTiles(const Wall& wall) {
    assert(isSetup);

    std::vector<Cell> adjacentTiles{};
    adjacentTiles.reserve(2);
    auto [x, y] = wall.worldPos;

    if (wall.orientation == Orientation::Horizontal) {
        if (Root::zoo()->world->isPositionInMap(Vector2{x - 0.5f, y - 1.0f})) adjacentTiles.emplace_back(Vector2{x - 0.5f, y - 1.0f});
        if (Root::zoo()->world->isPositionInMap(Vector2{x - 0.5f, y})) adjacentTiles.emplace_back(Vector2{x - 0.5f, y});
    } else {
        if (Root::zoo()->world->isPositionInMap(Vector2{x - 1.0f, y - 0.5f})) adjacentTiles.emplace_back(Vector2{x - 1.0f, y - 0.5f});
        if (Root::zoo()->world->isPositionInMap(Vector2{x, y - 0.5f})) adjacentTiles.emplace_back(Vector2{x, y - 0.5f});
    }

    return adjacentTiles;
}

std::array<Cell, 2> WallGrid::getWallVertices(const Wall &wall) {
    assert(isSetup);
    switch (wall.orientation) {
        case Orientation::Horizontal:
            return {
                flr(Vector2{wall.worldPos.x - 0.5f, wall.worldPos.y}),
                flr(Vector2{wall.worldPos.x + 0.5f, wall.worldPos.y})
            };
        case Orientation::Vertical:
            return {
                flr(Vector2{wall.worldPos.x, wall.worldPos.y - 0.5f}),
                flr(Vector2{wall.worldPos.x, wall.worldPos.y + 0.5f})
            };
    }
}

std::vector<Wall*> WallGrid::getSurroundingWalls(Cell gridPos) {
    assert(isSetup);
    std::vector<Wall*> walls{};

    walls.push_back(getWallAtTile(gridPos, Side::North));
    walls.push_back(getWallAtTile(gridPos, Side::West));
    walls.push_back(getWallAtTile(gridPos - Cell{1, 1}, Side::South));
    walls.push_back(getWallAtTile(gridPos - Cell{1, 1}, Side::East));

    return walls;
}

bool WallGrid::isWallSloped(const Wall &wall) {
    assert(isSetup);
    auto& elevationGrid = Root::zoo()->world->elevationGrid;
    auto [v1, v2] = getWallVertices(wall);

    return elevationGrid->getElevationAtPos(v1) != elevationGrid->getElevationAtPos(v2);
}

Vector2 WallGrid::wallToWorldPosition(Cell gridPos, Orientation orientation) {
    if (orientation == Orientation::Horizontal) {
        return {float(gridPos.x) / 2.0f, float(gridPos.y)};
    } else {
        return {float(gridPos.x) / 2.0f, float(gridPos.y) + 0.5f};
    }
}

GridPos WallGrid::getGridPosition(Cell tilePos, Side side) {
    int x, y; Orientation orientation;
    switch (side) {
        case Side::North:
            x = tilePos.x * 2 + 1;
            y = tilePos.y;
            orientation = Orientation::Horizontal;
            break;
        case Side::East:
            x = tilePos.x * 2 + 2;
            y = tilePos.y;
            orientation = Orientation::Vertical;
            break;
        case Side::South:
            x = tilePos.x * 2 + 1;
            y = tilePos.y + 1;
            orientation = Orientation::Horizontal;
            break;
        case Side::West:
            x = tilePos.x * 2;
            y = tilePos.y;
            orientation = Orientation::Vertical;
            break;
    }
    return { x, y, orientation };
}

WallSpriteInfo WallGrid::getSpriteInfo(Wall& wall, bool isDoor) {
    if (wall.orientation == Orientation::Horizontal) {
        auto left = Root::zoo()->world->elevationGrid->getElevationAtPos({wall.worldPos.x - 0.5f, wall.worldPos.y});
        auto right = Root::zoo()->world->elevationGrid->getElevationAtPos({wall.worldPos.x + 0.5f, wall.worldPos.y});
        auto elevation = std::min(left, right);

        if (left == right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::Horizontal, elevation };
        if (left > right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::HillWest, elevation };
        if (left < right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::HillEast, elevation };
    } else {
        auto up = Root::zoo()->world->elevationGrid->getElevationAtPos({wall.worldPos.x, wall.worldPos.y - 0.5f});
        auto down = Root::zoo()->world->elevationGrid->getElevationAtPos({wall.worldPos.x, wall.worldPos.y + 0.5f});
        auto elevation = std::min(up, down);

        if (up == down) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorVertical : (unsigned int)WallSpriteIndex::Vertical, elevation };
        if (up > down) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorVertical : (unsigned int)WallSpriteIndex::HillNorth, elevation };
        if (up < down) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorVertical : (unsigned int)WallSpriteIndex::HillSouth, elevation + ELEVATION_HEIGHT };
    }

    return {(unsigned int)WallSpriteIndex::Horizontal, 0.0f };
}

bool WallGrid::shouldCheckForLoop(const Wall &wall) {
    auto adjacent = getAdjacentWalls(wall);
    if (adjacent.size() < 2) return false;

    if (wall.orientation == Orientation::Horizontal) {
        if (
            std::find_if(adjacent.begin(), adjacent.end(), [wall](Wall* a) { return a->worldPos.x > wall.worldPos.x; }) != adjacent.end() &&
            std::find_if(adjacent.begin(), adjacent.end(), [wall](Wall* a) { return a->worldPos.x < wall.worldPos.x; }) != adjacent.end()
        ) {
            return true;
        }
    } else {
        if (
            std::find_if(adjacent.begin(), adjacent.end(), [wall](Wall* a) { return a->worldPos.y > wall.worldPos.y; }) != adjacent.end() &&
            std::find_if(adjacent.begin(), adjacent.end(), [wall](Wall* a) { return a->worldPos.y < wall.worldPos.y; }) != adjacent.end()
        ) {
            return true;
        }
    }
    return false;
}

bool WallGrid::checkForLoop(Wall* startWall, Wall* currentWall, std::unordered_set<std::string> checkedWalls, unsigned int depth) {
    if (!currentWall) currentWall = startWall;

    // Expand current node
    checkedWalls.insert(currentWall->gridPos.toString());

    auto found = false;
    for (auto wall : getAdjacentWalls(*currentWall)) {
        if (wall == startWall && depth > 1) return true;

        if (!checkedWalls.contains(wall->gridPos.toString())) {
            found = checkForLoop(startWall, wall, checkedWalls, depth + 1);
        }
        if (found) break;
    }
    
    return found;
}

void WallGrid::serialise() {
    if (Root::saveManager().mode == SerialiseMode::Loading)
        cleanup();

    Root::saveManager().SerialiseValue("rows", rows);
    Root::saveManager().SerialiseValue("cols", cols);

    std::function<json()> get = [&](){
        std::vector<std::vector<json>> gridData;
        std::transform(grid.begin(), grid.end(), std::back_inserter(gridData), [](std::vector<Wall>& row) {
            std::vector<json> rowData;
            std::transform(row.begin(), row.end(), std::back_inserter(rowData), [](Wall& wall) {
                return json({
                    {"assetPath", wall.exists ? wall.data->assetPath : ""},
                    {"exists", wall.exists}, // TODO: might be redundant because of assetPath?
                    {"indestructable", wall.indestructable},
                    {"isDoor", wall.isDoor}
                });
            });
            return rowData;
        });

        return gridData;
    };
    std::function<void(json)> set = [&](json data){
        auto gridData = data.get<std::vector<std::vector<json>>>();

        for (int i = 0; i < cols * 2 + 1; i++) {
            auto orientation = (Orientation)(i % 2);
            grid.emplace_back();
            for (int j = 0; j < rows + int(orientation); j++) {
                auto worldPos = WallGrid::wallToWorldPosition({i, j}, orientation);
                auto wallData = gridData.at(i).at(j);
                auto exists = wallData.at("exists").get<bool>();

                grid.at(i).push_back({
                     exists ? &Registry::getWall(wallData.at("assetPath").get<std::string>()) : nullptr,
                     orientation,
                     worldPos,
                     {i, j},
                     exists,
                     wallData.at("indestructable").get<bool>(),
                     wallData.at("isDoor").get<bool>(),
                 });
            }
        }
    };
    Root::saveManager().SerialiseValue("grid", get, set);

    if (Root::saveManager().mode == SerialiseMode::Loading)
        isSetup = true;
}
