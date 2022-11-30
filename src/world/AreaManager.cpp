#include "AreaManager.h"
#include "Messenger.h"
#include "Root.h"
#include "util/uuid.h"
#include "Debug.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
#define ZOO_AREA "ZOO"

void AreaManager::setup () {
    TraceLog(LOG_TRACE, "Setting up areas");

    auto zooArea = std::make_unique<Area>(ZOO_AREA);
    auto zooTiles = floodFill(Cell{1, 1});
    assert(!zooTiles.empty());
    zooArea->tiles = zooTiles;
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
    if (areaATiles.size() + areaBTiles.size() > oldArea->tiles.size()) {
        TraceLog(LOG_WARNING, "False positive in loop check");
        return;
    }

    auto newArea = std::make_unique<Area>(uuid::generate());

    auto& larger = areaATiles.size() >= areaBTiles.size() ? areaATiles : areaBTiles;
    auto& smaller = areaATiles.size() < areaBTiles.size() ? areaATiles : areaBTiles;

    oldArea->tiles = larger;
    for (auto tile : larger) tileAreaMap[tile.toString()] = oldArea;
    newArea->tiles = smaller;
    for (auto tile : smaller) tileAreaMap[tile.toString()] = newArea.get();

    TraceLog(LOG_TRACE, "Registered new area with size: %u", newArea->tiles.size());

    areas.insert_or_assign(newArea->id, std::move(newArea));

    Messenger::fire(EventType::AreasUpdated);
}

void AreaManager::joinAreas(Wall &removedWall) {
    auto tiles = Root::zoo()->world->wallGrid->getAdjacentTiles(removedWall);
    assert(tiles.size() == 2);

    auto areaA = getAreaAtPosition(tiles[0]);
    auto areaB = getAreaAtPosition(tiles[1]);

    if (areaA == areaB)
        return;

    // If one of the areas is the main zoo area then ensure we keep it
    if (areaB->id == ZOO_AREA) {
        auto swap = areaA;
        areaA = areaB;
        areaB = swap;
    }

    // Delete area B and expand area A
    areaA->tiles.insert(areaA->tiles.end(), areaB->tiles.begin(), areaB->tiles.end());
    for (auto tile : areaB->tiles) tileAreaMap[tile.toString()] = areaA;
    for (const auto& areaConnection : areaB->connectedAreas) {
        const auto& [area, doors] = areaConnection;
        for (auto door : doors) {
            areaA->addAreaConnection(area, door);
            area->addAreaConnection(areaA, door);
        }
    }
    areas.erase(areaB->id);

    Messenger::fire(EventType::AreasUpdated);
}

void AreaManager::resetAreas() {
    // TOOD: What was this for?
}

std::vector<Area*> AreaManager::getAreas() const {
    assert(isSetup);
    // TODO: Cache so we don't have to do this every time?
    std::vector<Area*> a;
    for (const auto& [_, area] : areas) {
        a.push_back(area.get());
    }
    return a;
}

Area* AreaManager::getAreaById(std::string id) const {
    assert(isSetup);
    if (!areas.contains(id)) return nullptr;
    return areas.at(id).get();
}

Area* AreaManager::getAreaAtPosition(Vector2 pos) const {
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

std::list<Area*> AreaManager::BFS (Area* start, Area* end) const {
    std::map<std::string, int> dist;
    std::map<std::string, std::string> prev;

    dist[start->id] = 0;

    std::queue<Area*> queue;
    queue.push(start);

    int curDist = 1;
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        for (const auto& pair : current->connectedAreas) {
            auto neighbour = pair.first;

            if (dist.contains(neighbour->id) && curDist >= dist.at(neighbour->id)) continue;

            prev[neighbour->id] = current->id;
            dist[neighbour->id] = curDist;
            queue.push(neighbour);

            // Found end, construct path
            if (neighbour == end) {
                std::list<Area*> path{};
                path.push_back(end);
                std::string n = end->id;
                while (prev.contains(n) && n != start->id) {
                    n = prev[n];
                    path.push_back(getAreaById(n));
                }
                path.reverse();
                return path;
            }
        }

        curDist++;
    }

    return {};
}

void AreaManager::renderDebugAreaGrid() {
    assert(isSetup);
    for (auto& pair : areas) {
        auto& area = pair.second;
        for (auto& tile : area->tiles) {
            Debug::drawRect(tile, Cell{1, 1}, ColorAlpha(area->colour, 0.5f), true);
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
            std::transform(area->connectedAreas.begin(), area->connectedAreas.end(), std::back_inserter(connectedAreaSaveData), [](
                const std::pair<Area *const, std::unordered_set<Wall *>>& connection) {
                const auto& [connectedArea, doors] = connection;
                std::vector<Cell> doorGridPositions;
                std::transform(doors.begin(), doors.end(), std::back_inserter(doorGridPositions), [](Wall* door) {
                    return door->gridPos;
                });

                return json({
                    {"areaId", connectedArea->id},
                    {"doorGridPositions", doorGridPositions}
                });
            });

            return json({
                {"id", id},
                {"colour", area->colour},
                {"tiles", area->tiles},
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
            areaData.at("colour").get_to(area->colour);
            areaData.at("tiles").get_to(area->tiles);
            areas.insert_or_assign(id, std::move(area));
            for (auto tile : areas.at(id)->tiles) tileAreaMap.insert_or_assign(tile.toString(), areas.at(id).get());
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

#pragma clang diagnostic pop