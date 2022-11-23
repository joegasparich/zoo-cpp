#include "AreaManager.h"
#include "Messenger.h"
#include "Root.h"
#include "util/uuid.h"
#include "Debug.h"

#define ZOO_AREA "ZOO"

void AreaManager::setup () {
    TraceLog(LOG_TRACE, "Setting up areas");

    auto zooArea = std::make_unique<Area>(ZOO_AREA);
    auto zooTiles = floodFill(Cell{1, 1});
    assert(!zooTiles.empty());
    zooArea->m_tiles = zooTiles;
    areas.insert_or_assign(ZOO_AREA, std::move(zooArea));
    for (auto tile : zooTiles) tileAreaMap.insert_or_assign(tile.toString(), areas.at(ZOO_AREA).get());
    Messenger::fire(EventType::AreasUpdated);

    isSetup = true;
}

void AreaManager::cleanup () {
    // TODO
}

void AreaManager::formAreas(Wall &placedWall) {
    assert(placedWall.exists);

    std::vector<Cell> areaATiles;
    std::vector<Cell> areaBTiles;
    auto startTiles = Root::zoo()->world->wallGrid->getAdjacentTiles(placedWall);

    // ! Does not handle situations where final wall is placed on the edge of the map
    // Current solution is to ensure that the map is already surrounded by walls
    assert(startTiles.size() >= 2);

    areaATiles = floodFill(startTiles[0]);
    areaBTiles = floodFill(startTiles[1]);

    auto oldArea = tileAreaMap.at(startTiles[0].toString());

    // Return if areas weren't formed properly (false positive in loop check)
    if (areaATiles.size() + areaBTiles.size() > oldArea->m_tiles.size()) {
        TraceLog(LOG_WARNING, "False positive in loop check");
        return;
    }

    auto newArea = std::make_unique<Area>(uuid::generate());

    auto& larger = areaATiles.size() >= areaBTiles.size() ? areaATiles : areaBTiles;
    auto& smaller = areaATiles.size() < areaBTiles.size() ? areaATiles : areaBTiles;

    oldArea->m_tiles = larger;
    for (auto tile : larger) tileAreaMap[tile.toString()] = oldArea;
    newArea->m_tiles = smaller;
    for (auto tile : smaller) tileAreaMap[tile.toString()] = newArea.get();

    TraceLog(LOG_TRACE, "Registered new area with size: %u", newArea->m_tiles.size());

    areas.insert_or_assign(newArea->m_id, std::move(newArea));

    Messenger::fire(EventType::AreasUpdated);
}

void AreaManager::joinAreas(Wall &removedWall) {
    auto tiles = Root::zoo()->world->wallGrid->getAdjacentTiles(removedWall);
    assert(tiles.size() == 2);

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
    for (auto tile : areaB->m_tiles) tileAreaMap[tile.toString()] = areaA;
    for (const auto& areaConnection : areaB->m_connectedAreas) {
        const auto& [area, doors] = areaConnection;
        for (auto door : doors) {
            areaA->addAreaConnection(area, door);
            area->addAreaConnection(areaA, door);
        }
    }
    areas.erase(areaB->m_id);

    Messenger::fire(EventType::AreasUpdated);
}

void AreaManager::resetAreas() {
    // TOOD: What was this for?
}

std::vector<Area*> AreaManager::getAreas() {
    assert(isSetup);
    // TODO: Cache so we don't have to do this every time?
    std::vector<Area*> a;
    for (const auto& [_, area] : areas) {
        a.push_back(area.get());
    }
    return a;
}

Area* AreaManager::getAreaById(std::string id) {
    assert(isSetup);
    return areas[id].get();
}

Area* AreaManager::getAreaAtPosition(Vector2 pos) {
    assert(isSetup);
    if (!Root::zoo()->world->isPositionInMap(pos)) return nullptr;

    auto key = Cell{pos}.toString();
    assert(tileAreaMap.contains(key));
    return tileAreaMap.at(key);
}

std::vector<Cell> AreaManager::floodFill(Cell startTile) {
    std::unordered_set<Cell> tileSet;
    std::stack<Cell> openTiles;

    tileSet.insert(startTile);
    openTiles.push(startTile);

    while (!openTiles.empty()) {
        auto currentTile = openTiles.top();
        openTiles.pop();
        auto neighbours = Root::zoo()->world->getAccessibleAdjacentTiles(currentTile);

        for (auto neighbour : neighbours) {
            if (!tileSet.contains(neighbour)) {
                tileSet.insert(neighbour);
                openTiles.push(neighbour);
            }
        }
    }

    std::vector<Cell> tiles;
    tiles.assign(tileSet.begin(), tileSet.end());
    return tiles;
}

void AreaManager::renderDebugAreaGrid() {
    assert(isSetup);
    for (auto& pair : areas) {
        auto& area = pair.second;
        for (auto& tile : area->m_tiles) {
            Debug::drawRect(tile, Cell{1, 1}, ColorAlpha(area->m_colour, 0.5f), true);
        }
    }
}

void AreaManager::serialise () {
    std::function<json()> get = [&](){
        // TODO: Can I use ISerialisable here?

        // Fuck c++ is so verbose this is ridiculous
        // Give me my array.map() back
        std::vector<json> areaSaveData;
        std::transform(areas.begin(), areas.end(), std::back_inserter(areaSaveData), [](const std::pair<const std::string, std::unique_ptr<Area>>& pair) {
            const auto& [id, area] = pair;
            std::vector<json> connectedAreaSaveData;
            std::transform(area->m_connectedAreas.begin(), area->m_connectedAreas.end(), std::back_inserter(connectedAreaSaveData), [](
                const std::pair<Area *const, std::unordered_set<Wall *>>& connection) {
                const auto& [connectedArea, doors] = connection;
                std::vector<Cell> doorGridPositions;
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

        return areaSaveData;
    };
    std::function<void(json)> set = [&](json data){
        // Create areas
        for (auto& areaData : data) {
            auto id = areaData.at("id").get<std::string>();
            auto area = std::make_unique<Area>(id);
            areaData.at("colour").get_to(area->m_colour);
            areaData.at("tiles").get_to(area->m_tiles);
            areas.insert_or_assign(id, std::move(area));
            for (auto tile : areas.at(id)->m_tiles) tileAreaMap.insert_or_assign(tile.toString(), areas.at(id).get());
        }

        // Add connections once all areas have been created
        for (auto& areaData : data) {
            auto area = getAreaById(areaData.at("id").get<std::string>());
            for (auto& connectedAreaData : areaData.at("connectedAreas")) {
                auto connectedArea = getAreaById(connectedAreaData.at("areaId").get<std::string>());
                for (auto& doorGridPos : connectedAreaData.at("doorGridPositions")) {
                    auto door = Root::zoo()->world->wallGrid->getWallByGridPos(doorGridPos.get<Cell>());
                    area->addAreaConnection(connectedArea, door);
                }
            }
        }

        Messenger::fire(EventType::AreasUpdated);
    };
    Root::saveManager().SerialiseValue("areas", get, set);
}
