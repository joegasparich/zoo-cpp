#include <constants/world.h>
#include <util/uuid.h>
#include <util/util.h>
#include "Debug.h"
#include "World.h"
#include "entities/components/TileObjectComponent.h"

#define ZOO_AREA "ZOO"

World::World(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

void World::setup() {
    m_elevationGrid = std::make_unique<ElevationGrid>(m_width + 1, m_height + 1);
    m_elevationGrid->setup();

    m_biomeGrid = std::make_unique<BiomeGrid>(m_width * (int)BIOME_SCALE, m_height * (int)BIOME_SCALE);

    m_wallGrid = std::make_unique<WallGrid>(m_width, m_height);
    m_wallGrid->setup();

    auto zooArea = std::make_unique<Area>(ZOO_AREA);
    auto zooTiles = floodFill(glm::vec2{1, 1});
    zooArea->m_tiles = zooTiles;
    m_areas.insert_or_assign(ZOO_AREA, std::move(zooArea));
    for (auto tile : zooTiles) m_tileAreaMap.insert_or_assign(vecToString(tile), m_areas.at(ZOO_AREA).get());
    // Mediator.fire(WorldEvent.AREAS_UPDATED);
}

void World::cleanup() {
    m_biomeGrid->cleanup();
    m_elevationGrid->cleanup();
    m_wallGrid->cleanup();
}

void World::update() {

}

void World::postUpdate() {
    m_biomeGrid->postUpdate();
}

void World::render() {
    m_biomeGrid->render();
    m_elevationGrid->render();
    m_elevationGrid->renderDebug();
    m_wallGrid->render();

    for (auto& pair : m_areas) {
        auto& area = pair.second;
        for (auto& tile : area->m_tiles) {
            Debug::drawRect(tile, tile + glm::ivec2{1, 1}, {area->m_colour, 0.5f});
        }
    }
}

void World::registerTileObject(Entity *tileObject) {
    auto component = tileObject->getComponent<TileObjectComponent>();

    m_tileObjects.insert_or_assign(tileObject->getId(), tileObject);
    for (auto tile : component->getTiles()) {
        m_tileObjectMap.insert_or_assign(vecToString(tile), tileObject);
    }
}

void World::unregisterTileObject(Entity *tileObject) {

}

Entity *World::getTileObjectAtPosition(glm::vec2 pos) {
    glm::ivec2 tilePos = glm::floor(pos);
    if (!isPositionInMap(tilePos)) return nullptr;
    if (!m_tileObjectMap.contains(vecToString(tilePos))) return nullptr;
    return m_tileObjectMap.at(vecToString(tilePos));
}

bool World::isPositionInMap(glm::vec2 pos) const {
    return pos.x >= 0 && pos.x < (float)m_width && pos.y >= 0 && pos.y < (float)m_height;
}

glm::ivec2 World::getTileInDirection(glm::ivec2 tile, Side direction) {
    switch (direction) {
        case Side::North: return {tile.x, tile.y - 1};
        case Side::South: return {tile.x, tile.y + 1};
        case Side::West: return {tile.x - 1, tile.y};
        case Side::East: return {tile.x + 1, tile.y};
    }
}

Side World::getQuadrantAtPos(glm::vec2 pos) {
    auto xrel = fmod(pos.x + LARGER_THAN_WORLD, 1) - 0.5f;
    auto yrel = fmod(pos.y + LARGER_THAN_WORLD, 1) - 0.5f;

    if (yrel <= 0 && abs(yrel) >= abs(xrel)) return Side::North;
    if (xrel >= 0 && abs(xrel) >= abs(yrel)) return Side::East;
    if (yrel >= 0 && abs(yrel) >= abs(xrel)) return Side::South;
    if (xrel <= 0 && abs(xrel) >= abs(yrel)) return Side::West;
}

void World::formAreas(Wall &placedWall) {
    std::vector<glm::ivec2> areaATiles;
    std::vector<glm::ivec2> areaBTiles;
    auto startTiles = m_wallGrid->getAdjacentTiles(placedWall);

    // ! Does not handle situations where final wall is placed on the edge of the map
    // Current solution is to ensure that the map is already surrounded by walls
    if (startTiles.size() < 2) return;

    areaATiles = floodFill(startTiles[0]);
    areaBTiles = floodFill(startTiles[1]);

    std::cout << "area A: " << areaATiles.size() << std::endl;
    std::cout << "area B: " << areaBTiles.size() << std::endl;

    auto oldArea = m_tileAreaMap.at(vecToString(startTiles[0]));

    // Return if areas weren't formed properly (false positive in loop check)
    if (areaATiles.size() + areaBTiles.size() > oldArea->m_tiles.size()) {
        std::cout << "False positive in loop check" << std::endl;
        return;
    }

    auto newArea = std::make_unique<Area>(uuid::generate());

    auto& larger = areaATiles.size() >= areaBTiles.size() ? areaATiles : areaBTiles;
    auto& smaller = areaATiles.size() < areaBTiles.size() ? areaATiles : areaBTiles;

    oldArea->m_tiles = larger;
    for (auto tile : larger) m_tileAreaMap[vecToString(tile)] = oldArea;
    newArea->m_tiles = smaller;
    for (auto tile : smaller) m_tileAreaMap[vecToString(tile)] = newArea.get();

    m_areas.insert_or_assign(newArea->m_id, std::move(newArea));

    // Mediator.fire(WorldEvent.AREAS_UPDATED);

    return;
}

void World::joinAreas(Wall &removedWall) {

}

std::vector<Area *> World::getAreas() {
    return std::vector<Area *>();
}

Area &World::getAreaById(std::string id) {
//    return <#initializer#>;
}

std::vector<glm::ivec2> World::floodFill(glm::ivec2 startTile) {
    std::unordered_set<glm::ivec2> tileSet;
    std::stack<glm::ivec2> openTiles;

    tileSet.insert(startTile);
    openTiles.push(startTile);

    while (!openTiles.empty()) {
        auto currentTile = openTiles.top();
        openTiles.pop();
        auto neighbours = getAccessibleAdjacentTiles(currentTile);

        for (auto neighbour : neighbours) {
            if (!tileSet.contains(neighbour)) {
                tileSet.insert(neighbour);
                openTiles.push(neighbour);
            }
        }
    }

    std::vector<glm::ivec2> tiles;
    tiles.assign(tileSet.begin(), tileSet.end());
    return tiles;
}

std::vector<glm::ivec2> World::getAccessibleAdjacentTiles(glm::ivec2 tile) {
    if (!isPositionInMap(tile)) return{};

    auto allSides = {Side::North, Side::West, Side::South, Side::East};
    std::vector<glm::ivec2> adjacentTiles;

    for (auto side : allSides) {
        if (
            !m_wallGrid->getWallAtTile(tile, side)->exists &&
            isPositionInMap(getTileInDirection(tile, side))
        ) {
            adjacentTiles.push_back(getTileInDirection(tile, side));
        }
    }

    return adjacentTiles;
}

void World::resetAreas() {

}

json World::save() {
    return json{
            {"elevation", m_elevationGrid->save()},
            {"biome", m_biomeGrid->save()},
            {"wall", m_wallGrid->save()}
    };
}

void World::load(json saveData) {
    cleanup();

    // TODO: Check these exist first?
    m_elevationGrid->load(saveData["elevation"]);
    m_biomeGrid->load(saveData["biome"]);
    m_wallGrid->load(saveData["wall"]);
}
