#include "constants/world.h"
#include "util/uuid.h"
#include "Debug.h"
#include "World.h"
#include "entities/components/TileObjectComponent.h"
#include "Messenger.h"
#include "Root.h"
#include "Profiler.h"

World::World(unsigned int width, unsigned int height) : width(width), height(height) {}

World::~World() {
    cleanup();
}

void World::setup() {
    TraceLog(LOG_TRACE, "Generating world of size %ux%u", width, height);
    elevationGrid = std::make_unique<ElevationGrid>(width + 1, height + 1);
    elevationGrid->setup();

    biomeGrid = std::make_unique<BiomeGrid>(width * int(BIOME_SCALE), height * int(BIOME_SCALE));
    biomeGrid->setup();

    wallGrid = std::make_unique<WallGrid>(width, height);
    wallGrid->setup();

    pathGrid = std::make_unique<PathGrid>(width, height);
    pathGrid->setup();

    areaManager = std::make_unique<AreaManager>();
    areaManager->setup();

    pathfinder = std::make_unique<Pathfinder>(width, height);

    isSetup = true;

    TraceLog(LOG_TRACE, "Finished setting up world");
}

void World::cleanup() {
    biomeGrid->cleanup();
    elevationGrid->cleanup();
    wallGrid->cleanup();
    pathGrid->cleanup();

    isSetup = false;
}

void World::preUpdate () {}

void World::update() {}

void World::postUpdate() {
    biomeGrid->postUpdate();
}

void World::render() {
    assert(isSetup);
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
}

void World::renderDebug() {
    assert(isSetup);
    if (Root::zoo()->debugSettings.cellGrid) renderDebugCellGrid();
    if (Root::zoo()->debugSettings.biomeChunks) biomeGrid->renderChunkDebug();
    if (Root::zoo()->debugSettings.elevationGrid) elevationGrid->renderDebug();
    if (Root::zoo()->debugSettings.areaGrid) areaManager->renderDebugAreaGrid();
    if (Root::zoo()->debugSettings.pathfindingGrid) pathfinder->drawDebugGrid();
}

void World::registerTileObject(Entity *tileObject) {
    assert(isSetup);
    assert(tileObject);
    auto component = tileObject->getComponent<TileObjectComponent>();
    assert(component);
    assert(!component->getTiles().empty());

    tileObjects.insert_or_assign(tileObject->id, tileObject);
    for (auto tile : component->getTiles()) {
        tileObjectsMap.insert_or_assign(tile.toString(), tileObject);
    }

    if (component->data.solid) {
        auto tiles = component->getTiles();
        auto e = PlaceSolidEvent{EventType::PlaceSolid, &tiles};
        Messenger::fire(&e);
    }
}

void World::unregisterTileObject(Entity *tileObject) {
    assert(isSetup);
    assert(tileObject);
    auto component = tileObject->getComponent<TileObjectComponent>();
    assert(component);
    assert(!component->getTiles().empty());

    tileObjects.erase(tileObject->id);
    for (auto tile : component->getTiles()) {
        tileObjectsMap.erase(tile.toString());
    }
}

Entity* World::getTileObjectAtPosition(Vector2 pos) {
    assert(isSetup);
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

std::vector<Cell> World::getAccessibleAdjacentTiles(Cell tile) {
    assert(wallGrid->isSetup);
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

// TODO (optimisation): cache tile cost grids (paths, no water, etc.)
int World::getTileWalkability(Cell pos) {
    assert(isSetup);
    if (tileObjectsMap.contains(pos.toString()) && tileObjectsMap.at(pos.toString())->getComponent<TileObjectComponent>()->data.solid)
        return 0;

    if (elevationGrid->getTileBaseElevation(pos) < 0)
        return 0;

    if (pathGrid->getPathAtTile(pos)->exists)
        return 1;

    return 3;
}

void World::renderDebugCellGrid() {
    assert(isSetup);
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

void World::serialise() {
    Root::saveManager().SerialiseDeep("elevationGrid", elevationGrid.get());
    Root::saveManager().SerialiseDeep("biomeGrid", biomeGrid.get());
    Root::saveManager().SerialiseDeep("wallGrid", wallGrid.get());
    Root::saveManager().SerialiseDeep("pathGrid", pathGrid.get());
    Root::saveManager().SerialiseDeep("areaManager", areaManager.get());

    if (Root::saveManager().mode == SerialiseMode::Loading)
        isSetup = true;
}

unsigned int World::getWidth () const {
    return width;
}

unsigned int World::getHeight () const {
    return height;
}
