#include "stb_image/stb_image.h"

#include <Zoo.h>
#include <util/util.h>
#include <constants/assets.h>
#include "WallGrid.h"
#include "Messenger.h"

WallGrid::WallGrid(unsigned int cols, unsigned int rows) : m_cols(cols), m_rows(rows) {}

void WallGrid::setup() {
    m_va = std::make_unique<VertexArray>();
    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->pushFloat(3); // pos
    m_layout->pushFloat(3); // texCoord
    m_layout->pushFloat(3); // colour
    m_shader = std::make_unique<Shader>("./shaders/ArrayTextureVertex.shader", "./shaders/ArrayTextureFragment.shader");
    m_textureArray = std::make_unique<ArrayTexture>(128, 32, MAX_WALLS);

    auto walls = Registry::getAllWalls();

    for(auto wall : walls) {
        m_textureArray->pushTexture(*AssetManager::getSpriteSheet(wall->spriteSheetPath)->m_image);
    }

    for (int i = 0; i < m_cols * 2 + 1; i++) {
        auto orientation = (Orientation)(i % 2);
        m_grid.emplace_back();
        for (int j = 0; j < m_rows + (int)orientation; j++) {
            auto worldPos = WallGrid::wallToWorldPosition({i, j}, orientation);
            m_grid.at(i).push_back({
                nullptr,
                orientation,
                worldPos,
                {i, j},
                false,
                false
            });
        }
    }

    regenerateMesh();

    m_elevationListenerHandle = Messenger::on(EventType::ElevationUpdated, [this](json& data) {
        regenerateMesh();
    });

    m_isSetup = true;
}

void WallGrid::cleanup() {
    m_grid.clear();
    m_cols = 0;
    m_rows = 0;

    Messenger::unsubscribe(EventType::ElevationUpdated, m_elevationListenerHandle);

    m_isSetup = false;
}

void WallGrid::render() {
    m_va->bind();
    m_ib->bind();
    m_shader->bind();
    m_textureArray->bind();

    auto mvp = Renderer::getMVPMatrix(glm::vec2{0.0f, 0.0f}, 0.0f, 0.0f, glm::vec3{1.0f}, true);

    m_shader->setUniformMat4f("u_MVP", mvp);
    m_shader->setUniform1i("u_Textures", 0);

    Renderer::draw(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
}

// TODO: Chunk it?
void WallGrid::regenerateMesh() {
    float textureIndex = 0.0f;

    std::vector<ArrayTextureVertex> vertices{};
    std::vector<std::array<unsigned int, 6>> indices{};

    for (int i = 0; i < m_cols * 2 + 1; i++) {
        auto orientation = (Orientation)(i % 2);
        for (int j = 0; j < m_rows + (int)orientation; j++) {
            auto& wall = m_grid.at(i).at(j);
            if (wall.exists) {
                auto [ spriteIndex, elevation ] = WallGrid::getSpriteInfo(wall);
                auto pos = orientation == Orientation::Horizontal ? glm::vec2{i / 2.0f, j} : glm::vec2{i / 2.0f, j + 1};
                pos -= glm::vec2{0.5f, 2.0f + elevation}; // Offset cell size
                auto texCoord = AssetManager::getSpriteSheet(wall.data->spriteSheetPath)->getTexCoords(spriteIndex);
                auto depth = Renderer::getDepth(wall.worldPos.y);

                vertices.push_back({glm::vec3{pos + glm::vec2{0.0f, 0.0f}, depth}, {texCoord[0], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{1.0f, 0.0f}, depth}, {texCoord[1], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{1.0f, 2.0f}, depth}, {texCoord[2], textureIndex}, glm::vec3{1.0f}});
                vertices.push_back({glm::vec3{pos + glm::vec2{0.0f, 2.0f}, depth}, {texCoord[3], textureIndex}, glm::vec3{1.0f}});
                unsigned int base = indices.size() * 4;
                indices.push_back({
                      base, base + 1, base + 2,
                      base + 2, base + 3, base
                });
            }
        }
    }

    m_vb = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(ArrayTextureVertex));
    m_va->addBuffer(*m_vb, *m_layout);
    m_ib = std::make_unique<IndexBuffer>(&indices[0], (unsigned int)indices.size() * 6);
    m_numIndices = indices.size() * 6;
}

Wall* WallGrid::placeWallAtTile(WallData* data, glm::ivec2 tilePos, Side side) {
    if (!m_isSetup) return nullptr;
    if (!isWallPosInMap(tilePos, side)) return nullptr;
    if (getWallAtTile(tilePos, side)->exists) return nullptr;

    auto [ x, y, orientation ] = WallGrid::getGridPosition(tilePos, side);

    m_grid.at(x).at(y) = Wall{
            data,
            orientation,
            WallGrid::wallToWorldPosition({x, y}, orientation),
            {x, y},
            true,
            false
    };

    auto& wall = m_grid.at(x).at(y);

//    this.updatePathfindingAtWall(tilePos, side);

//    Messenger.fire(WorldEvent.PLACE_SOLID, { position: Wall.wallToWorldPos(new Vector(x, y), orientation) });

   if (shouldCheckForLoop(wall) && checkForLoop(&wall)) {
        Zoo::zoo->m_world->formAreas(m_grid.at(x).at(y));
   }

    regenerateMesh();

    return &wall;
}

void WallGrid::deleteWall(Wall wall) {
    if (!m_isSetup) return;

    this->deleteWallAtTile(
        glm::floor(wall.worldPos),
        wall.orientation == Orientation::Horizontal ? Side::North : Side::West
    );
}

void WallGrid::deleteWallAtTile(glm::ivec2 tilePos, Side side) {
    if (!m_isSetup) return;
    if (!isWallPosInMap(tilePos, side)) return;

    // Get grid position
    auto [ x, y, orientation ] = WallGrid::getGridPosition(tilePos, side);
    if (!m_grid[x][y].exists || m_grid[x][y].indestructable) return;

    // Set to blank wall
    m_grid[x][y] = Wall{
            {},
            orientation,
            WallGrid::wallToWorldPosition({x, y}, orientation),
            {x, y},
            false,
            false
    };

//    auto [tile1, tile2] = getAdjacentTiles(m_grid[x][y]);

//    if (Game.world.getAreaAtPosition(tile1) !== Game.world.getAreaAtPosition(tile2)) {
//        Game.world.joinAreas(this.wallGrid[x][y]);
//    }

//    // Update pathfinding information
//    if (side === Side.North && tilePos.y > 0) {
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y)),
//        );
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y - 1),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y - 1)),
//        );
//    }
//    if (side === Side.South && tilePos.y < Game.map.rows - 1) {
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y)),
//        );
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y + 1),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y + 1)),
//        );
//    }
//    if (side === Side.West && tilePos.x > 0) {
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y)),
//        );
//        Game.map.setTileAccess(
//                new Vector(tilePos.x - 1, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x - 1, tilePos.y)),
//        );
//    }
//    if (side === Side.East && tilePos.x < Game.map.cols - 1) {
//        Game.map.setTileAccess(
//                new Vector(tilePos.x, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x, tilePos.y)),
//        );
//        Game.map.setTileAccess(
//                new Vector(tilePos.x + 1, tilePos.y),
//                this.getWalledSides(new Vector(tilePos.x + 1, tilePos.y)),
//        );
//    }
}

void WallGrid::placeDoor(Wall* wall) {
    wall->isDoor = true;

    auto adjacentTiles = getAdjacentTiles(*wall);
    if (adjacentTiles.size() < 2) return;

    auto areaA = Zoo::zoo->m_world->getAreaAtPosition(adjacentTiles[0]);
    auto areaB = Zoo::zoo->m_world->getAreaAtPosition(adjacentTiles[1]);

    areaA->addAreaConnection(areaB, wall);
    areaB->addAreaConnection(areaA, wall);

    regenerateMesh();
}

void WallGrid::removeDoor(Wall* wall) {
    wall->isDoor = false;

    auto adjacentTiles = getAdjacentTiles(*wall);
    if (adjacentTiles.size() < 2) return;

    auto areaA = Zoo::zoo->m_world->getAreaAtPosition(adjacentTiles[0]);
    auto areaB = Zoo::zoo->m_world->getAreaAtPosition(adjacentTiles[1]);
    
    areaA->removeAreaConnection(areaB, wall);
    areaB->removeAreaConnection(areaA, wall);

    regenerateMesh();
}

bool WallGrid::isWallPosInMap(glm::ivec2 tilePos, Side side) const {
    if (!m_isSetup) return false;

    return (
        Zoo::zoo->m_world->isPositionInMap(tilePos) ||
        (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2{0, 1}) && side == Side::South) ||
        (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2{0, -1}) && side == Side::North) ||
        (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2{1, 0}) && side == Side::East) ||
        (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2{-1, 0}) && side == Side::West)
    );
}

bool WallGrid::isWallGridPosInMap(glm::ivec2 gridPos) {
    if (!m_isSetup) return false;

    return gridPos.x >= 0 && gridPos.x < m_grid.size() && gridPos.y >= 0 && gridPos.y < m_grid[gridPos.x].size();
}

Wall* WallGrid::getWallAtTile(glm::ivec2 tilePos, Side side) {
    if (!m_isSetup) return nullptr;

    if (!isWallPosInMap(tilePos, side)) {
        // Invert position and side if tile m_pos is correct but on the outside of the map
        if (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2(0, 1)) && side == Side::South) {
            tilePos = tilePos + glm::ivec2(0, 1);
            side = Side::North;
        } else if (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2(0, -1)) && side == Side::North) {
            tilePos = tilePos + glm::ivec2(0, -1);
            side = Side::South;
        } else if (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2(1, 0)) && side == Side::East) {
            tilePos = tilePos + glm::ivec2(-1, 0);
            side = Side::West;
        } else if (Zoo::zoo->m_world->isPositionInMap(tilePos + glm::ivec2(-1, 0)) && side == Side::West) {
            tilePos = tilePos + glm::ivec2(1, 0);
            side = Side::East;
        } else {
            // Position is outside map
            return nullptr;
        }
    }

    switch (side) {
        case Side::North:
            return &m_grid[tilePos.x * 2 + 1][tilePos.y];
        case Side::East:
            return &m_grid[tilePos.x * 2 + 2][tilePos.y];
        case Side::South:
            return &m_grid[tilePos.x * 2 + 1][tilePos.y + 1];
        case Side::West:
            return &m_grid[tilePos.x * 2][tilePos.y];
    }
}

Wall* WallGrid::getWallByGridPos(glm::ivec2 gridPos) {
    if (!m_isSetup) return nullptr;
    if (!isWallGridPosInMap(gridPos)) return nullptr;

    return &m_grid[gridPos.x][gridPos.y];
}

std::vector<Wall*> WallGrid::getAdjacentWalls(const Wall &wall) {
    if (!m_isSetup) return {};

    std::vector<Wall*> adjacentWalls{};
    auto x = wall.gridPos.x; auto y = wall.gridPos.y;

    if (wall.orientation == Orientation::Horizontal) {
        if (getWallByGridPos({x - 2, y}) && getWallByGridPos({x - 2, y})->exists) adjacentWalls.push_back(&m_grid[x - 2][y]);
        if (getWallByGridPos({x + 2, y}) && getWallByGridPos({x + 2, y})->exists) adjacentWalls.push_back(&m_grid[x + 2][y]);
        if (getWallByGridPos({x - 1, y}) && getWallByGridPos({x - 1, y})->exists) adjacentWalls.push_back(&m_grid[x - 1][y]);
        if (getWallByGridPos({x + 1, y}) && getWallByGridPos({x + 1, y})->exists) adjacentWalls.push_back(&m_grid[x + 1][y]);
        if (getWallByGridPos({x - 1, y - 1}) && getWallByGridPos({x - 1, y - 1})->exists) adjacentWalls.push_back(&m_grid[x - 1][y - 1]);
        if (getWallByGridPos({x + 1, y - 1}) && getWallByGridPos({x + 1, y - 1})->exists) adjacentWalls.push_back(&m_grid[x + 1][y - 1]);
    } else {
        if (getWallByGridPos({x - 1, y}) && getWallByGridPos({x - 1, y})->exists) adjacentWalls.push_back(&m_grid[x - 1][y]);
        if (getWallByGridPos({x + 1, y}) && getWallByGridPos({x + 1, y})->exists) adjacentWalls.push_back(&m_grid[x + 1][y]);
        if (getWallByGridPos({x - 1, y + 1}) && getWallByGridPos({x - 1, y + 1})->exists) adjacentWalls.push_back(&m_grid[x - 1][y + 1]);
        if (getWallByGridPos({x + 1, y + 1}) && getWallByGridPos({x + 1, y + 1})->exists) adjacentWalls.push_back(&m_grid[x + 1][y + 1]);
        if (getWallByGridPos({x, y + 1}) && getWallByGridPos({x, y + 1})->exists) adjacentWalls.push_back(&m_grid[x][y + 1]);
        if (getWallByGridPos({x, y - 1}) && getWallByGridPos({x, y - 1})->exists) adjacentWalls.push_back(&m_grid[x][y - 1]);
    }

    return adjacentWalls;
}

std::vector<glm::ivec2> WallGrid::getAdjacentTiles(const Wall& wall) {
    if (!m_isSetup) return {};

    std::vector<glm::ivec2> adjacentTiles{};
    adjacentTiles.reserve(2);
    auto x = wall.worldPos.x; auto y = wall.worldPos.y;

    if (wall.orientation == Orientation::Horizontal) {
        if (Zoo::zoo->m_world->isPositionInMap(glm::vec2(x - 0.5f, y - 1.0f))) adjacentTiles.emplace_back(glm::vec2(x - 0.5f, y - 1.0f));
        if (Zoo::zoo->m_world->isPositionInMap(glm::vec2(x - 0.5f, y))) adjacentTiles.emplace_back(glm::vec2(x - 0.5f, y));
    } else {
        if (Zoo::zoo->m_world->isPositionInMap(glm::vec2(x - 1.0f, y - 0.5f))) adjacentTiles.emplace_back(glm::vec2(x - 1.0f, y - 0.5f));
        if (Zoo::zoo->m_world->isPositionInMap(glm::vec2(x, y - 0.5f))) adjacentTiles.emplace_back(glm::vec2(x, y - 0.5f));
    }

    return adjacentTiles;
}

std::array<glm::ivec2, 2> WallGrid::getWallVertices(const Wall &wall) {
    switch (wall.orientation) {
        case Orientation::Horizontal:
            return {
                glm::floor(glm::vec2{wall.worldPos.x - 0.5f, wall.worldPos.y}),
                glm::floor(glm::vec2{wall.worldPos.x + 0.5f, wall.worldPos.y})
            };
        case Orientation::Vertical:
            return {
                glm::floor(glm::vec2{wall.worldPos.x, wall.worldPos.y - 0.5f}),
                glm::floor(glm::vec2{wall.worldPos.x, wall.worldPos.y + 0.5f})
            };
    }
}

std::vector<Wall*> WallGrid::getSurroundingWalls(glm::ivec2 gridPos) {
    std::vector<Wall*> walls{};

    walls.push_back(getWallAtTile(gridPos, Side::North));
    walls.push_back(getWallAtTile(gridPos, Side::West));
    walls.push_back(getWallAtTile(gridPos - glm::ivec2{1.0f, 1.0f}, Side::South));
    walls.push_back(getWallAtTile(gridPos - glm::ivec2{1.0f, 1.0f}, Side::East));

    return walls;
}

bool WallGrid::isWallSloped(const Wall &wall) {
    auto& elevationGrid = Zoo::zoo->m_world->m_elevationGrid;
    auto [v1, v2] = getWallVertices(wall);

    return elevationGrid->getElevationAtPos(v1) != elevationGrid->getElevationAtPos(v2);
}

glm::vec2 WallGrid::wallToWorldPosition(glm::ivec2 gridPos, Orientation orientation) {
    if (orientation == Orientation::Horizontal) {
        return {(float)gridPos.x / 2.0f, (float)gridPos.y};
    } else {
        return {(float)gridPos.x / 2.0f, (float)gridPos.y + 0.5f};
    }
}

GridPos WallGrid::getGridPosition(glm::ivec2 tilePos, Side side) {
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
        auto left = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({wall.worldPos.x - 0.5f, wall.worldPos.y});
        auto right = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({wall.worldPos.x + 0.5f, wall.worldPos.y});
        auto elevation = std::min(left, right);

        if (left == right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::Horizontal, elevation };
        if (left > right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::HillWest, elevation };
        if (left < right) return { isDoor || wall.isDoor ? (unsigned int)WallSpriteIndex::DoorHorizontal : (unsigned int)WallSpriteIndex::HillEast, elevation };
    } else {
        auto up = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({wall.worldPos.x, wall.worldPos.y - 0.5f});
        auto down = Zoo::zoo->m_world->m_elevationGrid->getElevationAtPos({wall.worldPos.x, wall.worldPos.y + 0.5f});
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
    checkedWalls.insert(vecToString(currentWall->gridPos));

    auto found = false;
    for (auto wall : getAdjacentWalls(*currentWall)) {
        if (wall == startWall && depth > 1) return true;

        if (!checkedWalls.contains(vecToString(wall->gridPos))) {
            found = checkForLoop(startWall, wall, checkedWalls, depth + 1);
        }
        if (found) break;
    }
    
    return found;
}

json WallGrid::save() {
    std::vector<std::vector<json>> saveData;
    std::transform(m_grid.begin(), m_grid.end(), std::back_inserter(saveData), [](std::vector<Wall>& row) {
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

    return json({
        {"grid", saveData},
        {"cols", m_cols},
        {"rows", m_rows}
    });
}

void WallGrid::load(json saveData) {
    cleanup();

    saveData.at("cols").get_to(m_cols);
    saveData.at("rows").get_to(m_rows);

    auto gridData = saveData.at("grid").get<std::vector<std::vector<json>>>();

    for (int i = 0; i < m_cols * 2 + 1; i++) {
        auto orientation = (Orientation)(i % 2);
        m_grid.emplace_back();
        for (int j = 0; j < m_rows + (int)orientation; j++) {
            auto worldPos = WallGrid::wallToWorldPosition({i, j}, orientation);
            auto wallData = gridData.at(i).at(j);
            auto exists = wallData.at("exists").get<bool>();

            m_grid.at(i).push_back({
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

    regenerateMesh();

    m_isSetup = true;
}
