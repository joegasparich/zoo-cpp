#include "constants/world.h"
#include "util/uuid.h"
#include "util/util.h"
#include "Debug.h"
#include "World.h"
#include "entities/components/TileObjectComponent.h"
#include "Messenger.h"
#include "Root.h"
#include "Profiler.h"

#define ZOO_AREA "ZOO"

World::World(unsigned int width, unsigned int height) : width(width), height(height) {}

World::~World() {
    cleanup();
}

void World::setup() {
    elevationGrid = std::make_unique<ElevationGrid>(width + 1, height + 1);
    elevationGrid->setup();

    biomeGrid = std::make_unique<BiomeGrid>(width * int(BIOME_SCALE), height * int(BIOME_SCALE));
    biomeGrid->setup();

    wallGrid = std::make_unique<WallGrid>(width, height);
    wallGrid->setup();

    pathGrid = std::make_unique<PathGrid>(width, height);
    pathGrid->setup();

    auto zooArea = std::make_unique<Area>(ZOO_AREA);
    auto zooTiles = floodFill(Cell{1, 1});
    zooArea->m_tiles = zooTiles;
    areas.insert_or_assign(ZOO_AREA, std::move(zooArea));
    for (auto tile : zooTiles) tileAreaMap.insert_or_assign(tile.toString(), areas.at(ZOO_AREA).get());
    Messenger::fire(EventType::AreasUpdated);

    pathfinder = std::make_unique<Pathfinder>(width, height);
}

void World::cleanup() {
    biomeGrid->cleanup();
    elevationGrid->cleanup();
    wallGrid->cleanup();
    pathGrid->cleanup();
}

void World::update() {

}

void World::postUpdate() {
    biomeGrid->postUpdate();
}

void World::render() {
    Profiler::startTimer("biome");
    biomeGrid->render();
    Profiler::stopTimer("biome");
    Profiler::startTimer("path");
    pathGrid->render();
    Profiler::stopTimer("path");
    Profiler::startTimer("elevation");
    elevationGrid->render();
    Profiler::stopTimer("elevation");
    Profiler::startTimer("wall");
    wallGrid->render();
    Profiler::stopTimer("wall");

    Profiler::startTimer("debug");
    // TODO: move somewhere
    if (Root::zoo()->debugSettings.cellGrid) renderDebugCellGrid();
    if (Root::zoo()->debugSettings.elevationGrid) elevationGrid->renderDebug();
    if (Root::zoo()->debugSettings.areaGrid) renderDebugAreaGrid();
    if (Root::zoo()->debugSettings.pathfindingGrid) pathfinder->drawDebugGrid();
    Profiler::stopTimer("debug");
}

void World::registerTileObject(Entity *tileObject) {
    auto component = tileObject->getComponent<TileObjectComponent>();

    tileObjects.insert_or_assign(tileObject->getId(), tileObject);
    for (auto tile : component->getTiles()) {
        tileObjectsMap.insert_or_assign(tile.toString(), tileObject);
    }
}

void World::unregisterTileObject(Entity *tileObject) {

}

Entity *World::getTileObjectAtPosition(Vector2 pos) {
    Cell tilePos = Cell{pos};
    if (!isPositionInMap(tilePos)) return nullptr;
    if (!tileObjectsMap.contains(tilePos.toString())) return nullptr;
    return tileObjectsMap.at(tilePos.toString());
}

bool World::isPositionInMap(Vector2 pos) const {
    return pos.x >= 0 && pos.x < float(width) && pos.y >= 0 && pos.y < float(height);
}

Cell World::getTileInDirection(Cell tile, Side direction) {
    switch (direction) {
        case Side::North: return {tile.x, tile.y - 1};
        case Side::South: return {tile.x, tile.y + 1};
        case Side::West: return {tile.x - 1, tile.y};
        case Side::East: return {tile.x + 1, tile.y};
    }
}

Side World::getQuadrantAtPos(Vector2 pos) {
    auto xrel = fmod(pos.x + LARGER_THAN_WORLD, 1) - 0.5f;
    auto yrel = fmod(pos.y + LARGER_THAN_WORLD, 1) - 0.5f;

    if (yrel <= 0 && abs(yrel) >= abs(xrel)) return Side::North;
    if (xrel >= 0 && abs(xrel) >= abs(yrel)) return Side::East;
    if (yrel >= 0 && abs(yrel) >= abs(xrel)) return Side::South;
    if (xrel <= 0 && abs(xrel) >= abs(yrel)) return Side::West;
}

void World::formAreas(Wall &placedWall) {
    std::vector<Cell> areaATiles;
    std::vector<Cell> areaBTiles;
    auto startTiles = wallGrid->getAdjacentTiles(placedWall);

    // ! Does not handle situations where final wall is placed on the edge of the map
    // Current solution is to ensure that the map is already surrounded by walls
    if (startTiles.size() < 2) return;

    areaATiles = floodFill(startTiles[0]);
    areaBTiles = floodFill(startTiles[1]);

    auto oldArea = tileAreaMap.at(startTiles[0].toString());

    // Return if areas weren't formed properly (false positive in loop check)
    if (areaATiles.size() + areaBTiles.size() > oldArea->m_tiles.size()) {
        std::cout << "False positive in loop check" << std::endl;
        return;
    }

    auto newArea = std::make_unique<Area>(uuid::generate());

    auto& larger = areaATiles.size() >= areaBTiles.size() ? areaATiles : areaBTiles;
    auto& smaller = areaATiles.size() < areaBTiles.size() ? areaATiles : areaBTiles;

    oldArea->m_tiles = larger;
    for (auto tile : larger) tileAreaMap[tile.toString()] = oldArea;
    newArea->m_tiles = smaller;
    for (auto tile : smaller) tileAreaMap[tile.toString()] = newArea.get();

    std::cout << "Registered new area with size" << newArea->m_tiles.size() << std::endl;

    areas.insert_or_assign(newArea->m_id, std::move(newArea));

    Messenger::fire(EventType::AreasUpdated);
}

void World::joinAreas(Wall &removedWall) {
    auto tiles = wallGrid->getAdjacentTiles(removedWall);
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
    for (auto tile : areaB->m_tiles) tileAreaMap[tile.toString()] = areaA;
    for (const auto& areaConnection : areaB->m_connectedAreas) {
         const auto& [area, doors] = areaConnection;
         for (auto door : doors) {
             areaA->addAreaConnection(area, door);
             area->addAreaConnection(areaA, door);
         }
     }
    // this.getExhibitByAreaId(area2.id)?.delete();
    areas.erase(areaB->m_id);

    Messenger::fire(EventType::AreasUpdated);
}

std::vector<Area*> World::getAreas() {
    // TODO: Store list so we don't have to do this every time?
    std::vector<Area*> a;
    for (const auto& [_, area] : areas) {
        a.push_back(area.get());
    }
    return a;
}

Area* World::getAreaById(std::string id) {
   return areas[id].get();
}

Area* World::getAreaAtPosition(Vector2 pos) {
    if (!isPositionInMap(pos)) return nullptr;

    auto key = Cell{pos}.toString();
    assert(tileAreaMap.contains(key));
    return tileAreaMap.at(key);
}

std::vector<Cell> World::floodFill(Cell startTile) {
    std::unordered_set<Cell> tileSet;
    std::stack<Cell> openTiles;

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

    std::vector<Cell> tiles;
    tiles.assign(tileSet.begin(), tileSet.end());
    return tiles;
}

std::vector<Cell> World::getAccessibleAdjacentTiles(Cell tile) {
    if (!isPositionInMap(tile)) return{};

    auto allSides = {Side::North, Side::West, Side::South, Side::East};
    std::vector<Cell> adjacentTiles;

    for (auto side : allSides) {
        if (
            !wallGrid->getWallAtTile(tile, side)->exists &&
            isPositionInMap(getTileInDirection(tile, side))
        ) {
            adjacentTiles.push_back(getTileInDirection(tile, side));
        }
    }

    return adjacentTiles;
}

void World::resetAreas() {

}

void World::renderDebugCellGrid() {
    // Horizontal
    for (auto i = 0; i < height + 1; i++) {
        Debug::drawLine(
            {0, float(i)},
            {float(height), float(i)},
            WHITE,
            true
        );
    }
    // Vertical
    for (auto i = 0; i < width + 1; i++) {
        Debug::drawLine(
            {float(i), 0},
            {float(i), float(width)},
            WHITE,
            true
        );
    }
}

void World::renderDebugAreaGrid() {
    for (auto& pair : areas) {
        auto& area = pair.second;
        for (auto& tile : area->m_tiles) {
            Debug::drawRect(tile, Cell{1, 1}, ColorAlpha(area->m_colour, 0.5f), true);
        }
    }
}

json World::save() {
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

    return json{
        {"elevation", elevationGrid->save()},
        {"biomes",    biomeGrid->save()},
        {"walls",     wallGrid->save()},
        {"paths",     pathGrid->save()},
        {"areas",     areaSaveData}
    };
}

void World::load(json saveData) {
    cleanup();

    // TODO: Check these exist first?
    elevationGrid->load(saveData.at("elevation"));
    biomeGrid->load(saveData.at("biomes"));
    wallGrid->load(saveData.at("walls"));
    pathGrid->load(saveData.at("paths"));

    // Create areas
    for (auto& areaData : saveData.at("areas")) {
        auto id = areaData.at("id").get<std::string>();
        auto area = std::make_unique<Area>(id);
        areaData.at("colour").get_to(area->m_colour);
        areaData.at("tiles").get_to(area->m_tiles);
        areas.insert_or_assign(id, std::move(area));
        for (auto tile : areas.at(id)->m_tiles) tileAreaMap.insert_or_assign(tile.toString(), areas.at(id).get());
    }

    // Add connections once all areas have been created
    for (auto& areaData : saveData.at("areas")) {
        auto area = getAreaById(areaData.at("id").get<std::string>());
        for (auto& connectedAreaData : areaData.at("connectedAreas")) {
            auto connectedArea = getAreaById(connectedAreaData.at("areaId").get<std::string>());
            for (auto& doorGridPos : connectedAreaData.at("doorGridPositions")) {
                auto door = wallGrid->getWallByGridPos(doorGridPos.get<Cell>());
                area->addAreaConnection(connectedArea, door);
            }
        }
    }

    Messenger::fire(EventType::AreasUpdated);
}
