#include <constants/world.h>
#include <util/uuid.h>
#include <util/util.h>
#include "Debug.h"
#include "World.h"
#include "entities/components/TileObjectComponent.h"

#define ZOO_AREA "ZOO"

World::World(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

World::~World() {
    cleanup();
}

void World::setup() {
    m_elevationGrid = std::make_unique<ElevationGrid>(m_width + 1, m_height + 1);
    m_elevationGrid->setup();

    m_biomeGrid = std::make_unique<BiomeGrid>(m_width * (int)BIOME_SCALE, m_height * (int)BIOME_SCALE);
    m_biomeGrid->setup();

    m_wallGrid = std::make_unique<WallGrid>(m_width, m_height);
    m_wallGrid->setup();

    m_pathGrid = std::make_unique<PathGrid>(m_width, m_height);
    m_pathGrid->setup();

    auto zooArea = std::make_unique<Area>(ZOO_AREA);
    auto zooTiles = floodFill(glm::vec2{1, 1});
    zooArea->m_tiles = zooTiles;
    m_areas.insert_or_assign(ZOO_AREA, std::move(zooArea));
    for (auto tile : zooTiles) m_tileAreaMap.insert_or_assign(vecToString(tile), m_areas.at(ZOO_AREA).get());
    // Messenger.fire(WorldEvent.AREAS_UPDATED);
}

void World::cleanup() {
    m_biomeGrid->cleanup();
    m_elevationGrid->cleanup();
    m_wallGrid->cleanup();
    m_pathGrid->cleanup();
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
    m_pathGrid->render();

// TODO: move somewhere
    for (auto& pair : m_areas) {
        auto& area = pair.second;
        for (auto& tile : area->m_tiles) {
            Debug::drawRect(tile, tile + glm::ivec2{1, 1}, {area->m_colour, 0.2f});
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

    // Messenger.fire(WorldEvent.AREAS_UPDATED);
}

void World::joinAreas(Wall &removedWall) {
    auto tiles = m_wallGrid->getAdjacentTiles(removedWall);
    if (tiles.size() != 2) return;

    auto areaA = getAreaAtPosition(tiles[0]);
    auto areaB = getAreaAtPosition(tiles[1]);

    // If one of the areas is the main zoo area then ensure we keep it
    if (areaB->m_id == ZOO_AREA) {
        auto swap = areaA;
        areaA = areaB;
        areaB = swap;
    }

    // Delete area B and expand area A
    areaA->m_tiles.insert(areaA->m_tiles.end(), areaB->m_tiles.begin(), areaB->m_tiles.end());
    for (auto tile : areaB->m_tiles) m_tileAreaMap[vecToString(tile)] = areaA;
    for (const auto& areaConnection : areaB->m_connectedAreas) {
         const auto& [area, doors] = areaConnection;
         for (auto door : doors) {
             areaA->addAreaConnection(area, door);
             area->addAreaConnection(areaA, door);
         }
     }
    // this.getExhibitByAreaId(area2.id)?.delete();
    m_areas.erase(areaB->m_id);

    // Messenger.fire(WorldEvent.AREAS_UPDATED);
}

std::vector<Area*> World::getAreas() {
    // TODO: Store list so we don't have to do this every time?
    std::vector<Area*> areas;
    for (const auto& [_, area] : m_areas) {
        areas.push_back(area.get());
    }
    return areas;
}

Area* World::getAreaById(std::string id) {
   return m_areas[id].get();
}

Area* World::getAreaAtPosition(glm::vec2 pos) {
    if (!isPositionInMap(pos)) return nullptr;
    assert(m_tileAreaMap.contains(vecToString((glm::ivec2)glm::floor(pos))));
    return m_tileAreaMap.at(vecToString((glm::ivec2)glm::floor(pos)));
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
    // Fuck c++ is so verbose this is ridiculous
    // Give me my array.map() back
    std::vector<json> areaSaveData;
    std::transform(m_areas.begin(), m_areas.end(), std::back_inserter(areaSaveData), [](const std::pair<const std::string, std::unique_ptr<Area>>& pair) {
        const auto& [id, area] = pair;
        std::vector<json> connectedAreaSaveData;
        std::transform(area->m_connectedAreas.begin(), area->m_connectedAreas.end(), std::back_inserter(connectedAreaSaveData), [](
            const std::pair<Area *const, std::unordered_set<Wall *>>& connection) {
            const auto& [connectedArea, doors] = connection;
            std::vector<glm::vec2> doorGridPositions;
            std::transform(doors.begin(), doors.end(), std::back_inserter(doorGridPositions), [](Wall* door) {
                return door->gridPos;
            });

            return json({
                {"areaId", connectedArea->m_id},
                {"doorGridPositions", doorGridPositions}
            });
        });

        return json({
            {"id", id},
            {"colour", area->m_colour},
            {"tiles", area->m_tiles},
            {"connectedAreas", connectedAreaSaveData}
        });
    });

    return json{
        {"elevation", m_elevationGrid->save()},
        {"biomes", m_biomeGrid->save()},
        {"walls", m_wallGrid->save()},
        {"areas", areaSaveData}
    };
}

void World::load(json saveData) {
    cleanup();

    // TODO: Check these exist first?
    m_elevationGrid->load(saveData.at("elevation"));
    m_biomeGrid->load(saveData.at("biomes"));
    m_wallGrid->load(saveData.at("walls"));

    // Create areas
    for (auto& areaData : saveData.at("areas")) {
        auto id = areaData.at("id").get<std::string>();
        auto area = std::make_unique<Area>(id);
        areaData.at("colour").get_to(area->m_colour);
        areaData.at("tiles").get_to(area->m_tiles);
        m_areas.insert_or_assign(id, std::move(area));
        for (auto tile : m_areas.at(id)->m_tiles) m_tileAreaMap.insert_or_assign(vecToString(tile), m_areas.at(id).get());
    }

    // Add connections once all areas have been created
    for (auto& areaData : saveData.at("areas")) {
        auto area = getAreaById(areaData.at("id").get<std::string>());
        for (auto& connectedAreaData : areaData.at("connectedAreas")) {
            auto connectedArea = getAreaById(connectedAreaData.at("areaId").get<std::string>());
            for (auto& doorGridPos : connectedAreaData.at("doorGridPositions")) {
                auto door = m_wallGrid->getWallByGridPos(doorGridPos.get<glm::ivec2>());
                area->addAreaConnection(connectedArea, door);
            }
        }
    }

    // Messenger.fire(WorldEvent.AREAS_UPDATED);
}
